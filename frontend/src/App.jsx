import React, { useEffect, useMemo, useState } from 'react'
import { ethers } from 'ethers'
import { abi } from './abi/OrderFulfillmentAbi'
import { CONTRACT_BYTECODE, CONTRACT_ABI } from './abi/OrderFulfillmentArtifact'

const STORAGE_KEY = 'order-fulfillment-address'

const getOrderIdStorageKey = (address) => `order-id-${address}`

export default function App() {
  const [provider, setProvider] = useState(null)
  const [signer, setSigner] = useState(null)
  const [account, setAccount] = useState('')
  const [contractAddress, setContractAddress] = useState(localStorage.getItem(STORAGE_KEY) || '')
  const [orderId, setOrderId] = useState('0')
  const [maxOrderId, setMaxOrderId] = useState('0')
  const [fulfillmentEth, setFulfillmentEth] = useState('0.001')
  const [shipmentEth, setShipmentEth] = useState('0.001')
  const [invoiceUri, setInvoiceUri] = useState('ipfs://Qm...')
  const [order, setOrder] = useState(null)
  const [statusMsg, setStatusMsg] = useState('')
  const [deploying, setDeploying] = useState(false)

  const contract = useMemo(() => {
    if (!signer || !ethers.isAddress(contractAddress)) return null
    return new ethers.Contract(contractAddress, abi, signer)
  }, [signer, contractAddress])

  // When contract address changes, load the associated orderId
  useEffect(() => {
    if (contractAddress && ethers.isAddress(contractAddress)) {
      localStorage.setItem(STORAGE_KEY, contractAddress)
      
      // Check if we have a saved order ID for this contract
      const savedOrderId = localStorage.getItem(getOrderIdStorageKey(contractAddress))
      setOrderId(savedOrderId || '0')
      
      // Try to fetch the max order ID (nextOrderId - 1)
      if (contract) {
        contract.nextOrderId().then(nextId => {
          const maxId = (BigInt(nextId) - 1n).toString()
          setMaxOrderId(maxId)
        }).catch(() => {
          setMaxOrderId('0')
        })
      } else {
        setMaxOrderId('0')
      }
      
      setOrder(null) // Clear order data when switching contracts
    }
  }, [contractAddress, contract])

  // Save orderId whenever it changes
  useEffect(() => {
    if (contractAddress && ethers.isAddress(contractAddress)) {
      localStorage.setItem(getOrderIdStorageKey(contractAddress), orderId)
    }
  }, [orderId, contractAddress])

  // Auto-load order whenever orderId or contract changes
  useEffect(() => {
    async function loadOrder() {
      if (!contract || orderId === null || orderId === '0') return
      try {
        const o = await contract.orders(BigInt(orderId))
        setOrder({
          status: Number(o.status),
          fulfillmentFee: o.fulfillmentFee.toString(),
          shipmentFee: o.shipmentFee.toString(),
          paid: o.paid.toString(),
          invoiceUri: o.invoiceUri,
        })
      } catch (err) {
        // Order doesn't exist yet, clear order display
        console.log('Order not found:', err.message)
        setOrder(null)
      }
    }
    loadOrder()
  }, [contract, orderId])

  async function connect() {
    if (!window.ethereum) {
      alert('MetaMask not detected. Please install it.')
      return
    }
    const p = new ethers.BrowserProvider(window.ethereum)
    await p.send('eth_requestAccounts', [])
    const s = await p.getSigner()
    setProvider(p)
    setSigner(s)
    setAccount(await s.getAddress())
  }

  async function handlePreviousOrder() {
    if (BigInt(orderId) > 0n) {
      const newId = (BigInt(orderId) - 1n).toString()
      setOrderId(newId)
      await handleLoadOrder(newId)
    }
  }

  async function handleNextOrder() {
    if (BigInt(orderId) < BigInt(maxOrderId)) {
      const newId = (BigInt(orderId) + 1n).toString()
      setOrderId(newId)
      await handleLoadOrder(newId)
    }
  }

  async function handleDeployContract() {
    if (!signer) {
      alert('Connect MetaMask first!')
      return
    }

    try {
      setDeploying(true)
      setStatusMsg('Checking network...')

      // Get current network
      let currentNetwork = await provider.getNetwork()
      
      // Switch to Sepolia if not already on it
      if (currentNetwork.chainId !== 11155111n) {
        setStatusMsg('Switching to Sepolia...')
        
        try {
          await window.ethereum.request({
            method: 'wallet_switchEthereumChain',
            params: [{ chainId: '0xaa36a7' }], // 11155111 in hex
          })
        } catch (switchError) {
          // If switch fails, try adding the network
          if (switchError.code === 4902) {
            await window.ethereum.request({
              method: 'wallet_addEthereumChain',
              params: [{
                chainId: '0xaa36a7',
                chainName: 'Sepolia',
                rpcUrls: ['https://eth-sepolia.g.alchemy.com/v2/demo'],
                nativeCurrency: { name: 'Sepolia ETH', symbol: 'ETH', decimals: 18 },
                blockExplorerUrls: ['https://sepolia.etherscan.io'],
              }],
            })
          } else {
            throw new Error(`Please manually switch to Sepolia network in MetaMask`)
          }
        }
        
        // Wait a bit for MetaMask to process the switch
        await new Promise(r => setTimeout(r, 1000))
        
        // Recreate provider and signer after network switch
        const newProvider = new ethers.BrowserProvider(window.ethereum)
        const newSigner = await newProvider.getSigner()
        setProvider(newProvider)
        setSigner(newSigner)
        
        setStatusMsg('Network switched to Sepolia!')
      }

      setStatusMsg('Deploying contract...')

      // Use fresh signer for deployment
      const deployerAddress = await signer.getAddress()
      const clientAddress = deployerAddress
      const courierAddress = deployerAddress

      // Create contract factory with current signer
      const factory = new ethers.ContractFactory(
        CONTRACT_ABI,
        CONTRACT_BYTECODE,
        signer
      )

      // Deploy contract
      const deployTx = await factory.deploy(clientAddress, courierAddress)
      console.log('Deployment tx hash:', deployTx.deploymentTransaction()?.hash)
      setStatusMsg('Waiting for confirmation...')

      // Wait for deployment to complete
      const deployedContract = await deployTx.waitForDeployment()
      const newContractAddress = await deployedContract.getAddress()

      console.log('Contract deployed at:', newContractAddress)

      // Save address to state and localStorage
      setContractAddress(newContractAddress)
      localStorage.setItem(STORAGE_KEY, newContractAddress)

      setStatusMsg(`✅ Contract deployed: ${newContractAddress}`)
      setDeploying(false)
    } catch (err) {
      console.error('Deployment error:', err)
      setStatusMsg(`❌ Error: ${err.message}`)
      setDeploying(false)
    }
  }

  async function handleCreateOrder() {
    if (!contract) return
    const tx = await contract.createOrder()
    setStatusMsg('Creating order...')
    const rc = await tx.wait()
    const ev = rc.logs.map(l => contract.interface.parseLog(l)).find(e => e && e.name === 'OrderCreated')
    if (ev) {
      const newOrderId = ev.args.orderId.toString()
      setOrderId(newOrderId)
      setStatusMsg(`Order created with ID ${newOrderId}`)
      
      // Update maxOrderId to the newly created order ID
      setMaxOrderId(newOrderId)
      
      // Pass the new order ID directly to load the new order's data
      await handleLoadOrder(newOrderId)
    } else {
      setStatusMsg('Order created (event not parsed). Check Etherscan.')
    }
  }

  async function handleSetPrice() {
    if (!contract) return
    const f = ethers.parseEther(fulfillmentEth)
    const s = ethers.parseEther(shipmentEth)
    const tx = await contract.setPrice(BigInt(orderId), f, s)
    setStatusMsg('Setting price...')
    await tx.wait()
    setStatusMsg('Price set.')
    await handleLoadOrder()
  }

  async function handleMarkProcessing() {
    if (!contract) return
    const tx = await contract.markProcessing(BigInt(orderId))
    setStatusMsg('Marking processing...')
    await tx.wait()
    setStatusMsg('Order marked as Processing.')
    await handleLoadOrder()
  }

  async function handleRequestPayment() {
    if (!contract) return
    const tx = await contract.requestPayment(BigInt(orderId))
    setStatusMsg('Requesting payment...')
    await tx.wait()
    setStatusMsg('Order set to AwaitingPayment.')
    await handleLoadOrder()
  }

  async function handlePay() {
    if (!contract) return
    // fetch due from chain
    const o = await contract.orders(BigInt(orderId))
    const due = (o.fulfillmentFee + o.shipmentFee)
    const tx = await contract.pay(BigInt(orderId), { value: due })
    setStatusMsg('Paying...')
    await tx.wait()
    setStatusMsg('Paid.')
    await handleLoadOrder()
  }

  async function handleUploadInvoice() {
    if (!contract) return
    try {
      const tx = await contract.uploadInvoice(BigInt(orderId), invoiceUri)
      setStatusMsg('Uploading invoice...')
      await tx.wait()
      setStatusMsg('Invoice uploaded. Closing and paying out...')
      
      // Automatically close and payout after invoice upload
      const closeTx = await contract.closeAndPayout(BigInt(orderId))
      await closeTx.wait()
      setStatusMsg('✅ Order closed and payouts sent.')
      
      // Refresh order data
      await handleLoadOrder()
    } catch (err) {
      setStatusMsg(`❌ Error: ${err.message}`)
    }
  }

  async function handleClose() {
    if (!contract) return
    const tx = await contract.closeAndPayout(BigInt(orderId))
    setStatusMsg('Closing and paying out...')
    await tx.wait()
    setStatusMsg('Closed and payouts sent.')
    await handleLoadOrder()
  }

  async function handleLoadOrder(queryOrderId = orderId) {
    if (!contract || queryOrderId === null) return
    try {
      const o = await contract.orders(BigInt(queryOrderId))
      setOrder({
        status: Number(o.status),
        fulfillmentFee: o.fulfillmentFee.toString(),
        shipmentFee: o.shipmentFee.toString(),
        paid: o.paid.toString(),
        invoiceUri: o.invoiceUri,
      })
    } catch (err) {
      console.log('Error loading order:', err.message)
      setOrder(null)
    }
  }

  const statusLabels = ['Registered','Priced','Processing','AwaitingPayment','Paid','Invoiced','Closed','Cancelled']

  return (
    <div style={{ fontFamily: 'Inter, system-ui, Arial', maxWidth: 900, margin: '24px auto', padding: 16 }}>
      <h2>Order Fulfillment smart contract</h2>

      <section style={{ border: '1px solid #ddd', padding: 12, borderRadius: 8, marginBottom: 16 }}>
        <button 
          onClick={connect} 
          disabled={!!account}
          style={{ fontWeight: 'bold', backgroundColor: '#667', color: 'white', padding: '8px 16px', border: 'none', borderRadius: 4, cursor: account ? 'not-allowed' : 'pointer' }}
        >
          {account ? `Connected: ${account.slice(0,6)}...${account.slice(-4)}` : 'Connect Wallet'}
        </button>
        <div style={{ marginTop: 8 }}>
          <label>Contract Address:&nbsp;</label>
          <input style={{ width: '480px' }} value={contractAddress} onChange={e=>setContractAddress(e.target.value)} placeholder="0x..." />
        </div>
        <small>Paste your Sepolia deployment address here.</small>
      </section>

      <section style={{ border: '1px solid #0a7', padding: 12, borderRadius: 8, marginBottom: 16, backgroundColor: '#f0fff5' }}>
        <h3>Deploy New Contract</h3>
        <p style={{ margin: '0 0 8px 0', color: '#555' }}>
          Deploy a new OrderFulfillment contract directly to Sepolia. This will automatically populate the contract address above.
        </p>
        <button 
          onClick={handleDeployContract} 
          disabled={!signer || deploying}
          style={{ fontWeight: 'bold', backgroundColor: '#0a7', color: 'white', padding: '8px 16px', border: 'none', borderRadius: 4, cursor: deploying ? 'not-allowed' : 'pointer' }}
        >
          {deploying ? 'Deploying...' : 'Deploy Contract'}
        </button>
        {deploying && <div style={{ marginTop: 8, color: '#666' }}>⏳ Transaction pending...</div>}
      </section>

      <section style={{ display:'grid', gridTemplateColumns:'1fr 1fr', gap: 12 }}>
        <div style={{ border:'1px solid #eee', padding:12, borderRadius:8 }}>
          <h4>Create / Price</h4>
          <div>
            <button 
              onClick={handleCreateOrder} 
              disabled={!contract}
              style={{ fontWeight: 'bold', backgroundColor: '#667', color: 'white', padding: '8px 16px', border: 'none', borderRadius: 4, cursor: !contract ? 'not-allowed' : 'pointer' }}
            >
              Create Order
            </button>
          </div>
          <div style={{ marginTop: 8 }}>
            <label>Order ID:&nbsp;</label>
            <div style={{ display: 'flex', alignItems: 'center', gap: 8 }}>
              {BigInt(maxOrderId) >= 1n && (
                <button 
                  onClick={handlePreviousOrder}
                  disabled={BigInt(orderId) === 0n}
                  style={{ fontWeight: 'bold', backgroundColor: '#667', color: 'white', padding: '4px 8px', border: 'none', borderRadius: 4, cursor: BigInt(orderId) === 0n ? 'not-allowed' : 'pointer' }}
                >
                  ←
                </button>
              )}
              <span style={{ fontWeight: 'bold', minWidth: '30px' }}>{orderId}</span>
              {BigInt(maxOrderId) >= 1n && (
                <button 
                  onClick={handleNextOrder}
                  disabled={BigInt(orderId) >= BigInt(maxOrderId)}
                  style={{ fontWeight: 'bold', backgroundColor: '#667', color: 'white', padding: '4px 8px', border: 'none', borderRadius: 4, cursor: BigInt(orderId) >= BigInt(maxOrderId) ? 'not-allowed' : 'pointer' }}
                >
                  →
                </button>
              )}
            </div>
          </div>
          <div style={{ marginTop: 8 }}>
            <label>Fulfillment (ETH):&nbsp;</label>
            <input value={fulfillmentEth} onChange={e=>setFulfillmentEth(e.target.value)} style={{ width: 120 }} />
          </div>
          <div style={{ marginTop: 8 }}>
            <label>Shipment (ETH):&nbsp;</label>
            <input value={shipmentEth} onChange={e=>setShipmentEth(e.target.value)} style={{ width: 120 }} />
          </div>
          <div style={{ marginTop: 8 }}>
            <button 
              onClick={handleSetPrice} 
              disabled={!contract}
              style={{ fontWeight: 'bold', backgroundColor: '#667', color: 'white', padding: '8px 16px', border: 'none', borderRadius: 4, cursor: !contract ? 'not-allowed' : 'pointer' }}
            >
              Set Price
            </button>
          </div>
        </div>

        <div style={{ border:'1px solid #eee', padding:12, borderRadius:8 }}>
          <h4>Process / Pay</h4>
          <div>
            <button 
              onClick={handleMarkProcessing} 
              disabled={!contract}
              style={{ fontWeight: 'bold', backgroundColor: '#667', color: 'white', padding: '8px 16px', border: 'none', borderRadius: 4, cursor: !contract ? 'not-allowed' : 'pointer' }}
            >
              Mark Processing
            </button>
          </div>
          <div style={{ marginTop: 8 }}>
            <button 
              onClick={handleRequestPayment} 
              disabled={!contract}
              style={{ fontWeight: 'bold', backgroundColor: '#667', color: 'white', padding: '8px 16px', border: 'none', borderRadius: 4, cursor: !contract ? 'not-allowed' : 'pointer' }}
            >
              Request Payment
            </button>
          </div>
          <div style={{ marginTop: 8 }}>
            <button 
              onClick={handlePay} 
              disabled={!contract}
              style={{ fontWeight: 'bold', backgroundColor: '#667', color: 'white', padding: '8px 16px', border: 'none', borderRadius: 4, cursor: !contract ? 'not-allowed' : 'pointer' }}
            >
              Pay (reads total)
            </button>
          </div>
        </div>

        <div style={{ border:'1px solid #eee', padding:12, borderRadius:8 }}>
          <h4>Upload Invoice</h4>
          <div>
            <label>Invoice URI:&nbsp;</label>
            <input value={invoiceUri} onChange={e=>setInvoiceUri(e.target.value)} style={{ width: 300 }} />
          </div>
          <div style={{ marginTop: 8 }}>
            <button 
              onClick={handleUploadInvoice} 
              disabled={!contract}
              style={{ fontWeight: 'bold', backgroundColor: '#667', color: 'white', padding: '8px 16px', border: 'none', borderRadius: 4, cursor: !contract ? 'not-allowed' : 'pointer' }}
            >
              Upload Invoice & Close Order
            </button>
          </div>
          <small style={{ color: '#666', display: 'block', marginTop: 8 }}>
            Uploads invoice and automatically closes order with payouts
          </small>
        </div>

        <div style={{ border:'1px solid #eee', padding:12, borderRadius:8 }}>
          <h4>Inspect Order</h4>
          {order ? (
            <div style={{ 
              fontFamily: 'monospace',
              opacity: order.status === 6 ? 1 : 1,
              backgroundColor: order.status === 6 ? '#d4f4dd' : 'transparent',
              padding: order.status === 6 ? 8 : 0,
              borderRadius: order.status === 6 ? 4 : 0
            }}>
              <div>Status: {statusLabels[order.status] ?? order.status}{order.status === 6 ? ' ✓' : ''}</div>
              <div>FulfillmentFee (wei): {order.fulfillmentFee}</div>
              <div>ShipmentFee (wei): {order.shipmentFee}</div>
              <div>Paid (wei): {order.paid}</div>
              <div>Invoice: {order.invoiceUri || '-'}</div>
            </div>
          ) : (
            <div style={{ color: '#999' }}>Create an order to see details here</div>
          )}
        </div>
      </section>

      {statusMsg && <div style={{ marginTop: 12, color:'#0a7' }}>{statusMsg}</div>}

      <div style={{ marginTop: 16, color: '#666' }}>
        <small>
          Tips: Paste your Sepolia contract address above. Use small ETH amounts (e.g., 0.001) while testing.
          Your connected account must have the provider role to use provider-only actions.
        </small>
      </div>
    </div>
  )
}
