import React, { useEffect, useMemo, useState } from 'react'
import { ethers } from 'ethers'
import { abi } from './abi/OrderFulfillmentAbi'
import { CONTRACT_BYTECODE, CONTRACT_ABI } from './abi/OrderFulfillmentArtifact'

const STORAGE_KEY = 'order-fulfillment-address'

export default function App() {
  const [provider, setProvider] = useState(null)
  const [signer, setSigner] = useState(null)
  const [account, setAccount] = useState('')
  const [contractAddress, setContractAddress] = useState(localStorage.getItem(STORAGE_KEY) || '')
  const [orderId, setOrderId] = useState('0')
  const [fulfillmentEth, setFulfillmentEth] = useState('0.001')
  const [shipmentEth, setShipmentEth] = useState('0.001')
  const [invoiceUri, setInvoiceUri] = useState('ipfs://Qm...')
  const [tracking, setTracking] = useState('')
  const [order, setOrder] = useState(null)
  const [statusMsg, setStatusMsg] = useState('')
  const [deploying, setDeploying] = useState(false)

  const contract = useMemo(() => {
    if (!signer || !ethers.isAddress(contractAddress)) return null
    return new ethers.Contract(contractAddress, abi, signer)
  }, [signer, contractAddress])

  useEffect(() => {
    if (contractAddress) localStorage.setItem(STORAGE_KEY, contractAddress)
  }, [contractAddress])

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

  async function handleDeployContract() {
    if (!signer) {
      alert('Connect MetaMask first!')
      return
    }

    try {
      setDeploying(true)
      setStatusMsg('Checking network...')

      // Verify connected network is Sepolia (chain ID 11155111)
      const network = await provider.getNetwork()
      if (network.chainId !== 11155111n) {
        setStatusMsg('Attempting to switch to Sepolia...')
        
        // Try to switch to Sepolia
        try {
          await window.ethereum.request({
            method: 'wallet_switchEthereumChain',
            params: [{ chainId: '0xaa36a7' }], // 11155111 in hex
          })
          setStatusMsg('Switched to Sepolia! Deploying...')
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
            setStatusMsg('Added Sepolia and switching... Deploying...')
          } else {
            throw new Error(`Please manually switch to Sepolia network in MetaMask`)
          }
        }
      }

      setStatusMsg('Deploying contract...')

      // Contract constructor arguments
      // Using connected account address for both client and courier
      const deployerAddress = await signer.getAddress()
      const clientAddress = deployerAddress
      const courierAddress = deployerAddress

      // Create contract factory
      const factory = new ethers.ContractFactory(
        CONTRACT_ABI,
        CONTRACT_BYTECODE,
        signer
      )

      // Deploy contract
      const contract = await factory.deploy(clientAddress, courierAddress)
      console.log('Deployment tx hash:', contract.deploymentTransaction()?.hash)
      setStatusMsg('Waiting for confirmation...')

      // Wait for deployment to complete
      const deployedContract = await contract.waitForDeployment()
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
      setOrderId(ev.args.orderId.toString())
      setStatusMsg(`Order created with ID ${ev.args.orderId}`)
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
    await tx.wait(); setStatusMsg('Price set.')
  }

  async function handleMarkProcessing() {
    if (!contract) return
    const tx = await contract.markProcessing(BigInt(orderId))
    setStatusMsg('Marking processing...')
    await tx.wait(); setStatusMsg('Order marked as Processing.')
  }

  async function handleRequestPayment() {
    if (!contract) return
    const tx = await contract.requestPayment(BigInt(orderId))
    setStatusMsg('Requesting payment...')
    await tx.wait(); setStatusMsg('Order set to AwaitingPayment.')
  }

  async function handlePay() {
    if (!contract) return
    // fetch due from chain
    const o = await contract.orders(BigInt(orderId))
    const due = (o.fulfillmentFee + o.shipmentFee)
    const tx = await contract.pay(BigInt(orderId), { value: due })
    setStatusMsg('Paying...')
    await tx.wait(); setStatusMsg('Paid.')
  }

  async function handleUploadInvoice() {
    if (!contract) return
    const tx = await contract.uploadInvoice(BigInt(orderId), invoiceUri)
    setStatusMsg('Uploading invoice...')
    await tx.wait(); setStatusMsg('Invoice uploaded.')
  }

  async function handleClose() {
    if (!contract) return
    const tx = await contract.closeAndPayout(BigInt(orderId))
    setStatusMsg('Closing and paying out...')
    await tx.wait(); setStatusMsg('Closed and payouts sent.')
  }

  async function handleSetTracking() {
    if (!contract) return
    const tx = await contract.setTracking(BigInt(orderId), tracking)
    setStatusMsg('Setting tracking...')
    await tx.wait(); setStatusMsg('Tracking set.')
  }

  async function handleLoadOrder() {
    if (!contract) return
    const o = await contract.orders(BigInt(orderId))
    setOrder({
      status: Number(o.status),
      fulfillmentFee: o.fulfillmentFee.toString(),
      shipmentFee: o.shipmentFee.toString(),
      paid: o.paid.toString(),
      tracking: o.tracking,
      invoiceUri: o.invoiceUri,
    })
  }

  const statusLabels = ['Registered','Priced','Processing','AwaitingPayment','Paid','Invoiced','Closed','Cancelled']

  return (
    <div style={{ fontFamily: 'Inter, system-ui, Arial', maxWidth: 900, margin: '24px auto', padding: 16 }}>
      <h2>Order Fulfillment DApp</h2>

      <section style={{ border: '1px solid #ddd', padding: 12, borderRadius: 8, marginBottom: 16 }}>
        <button onClick={connect} disabled={!!account}>
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
            <button onClick={handleCreateOrder} disabled={!contract}>Create Order</button>
          </div>
          <div style={{ marginTop: 8 }}>
            <label>Order ID:&nbsp;</label>
            <input value={orderId} onChange={e=>setOrderId(e.target.value)} style={{ width: 120 }} />
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
            <button onClick={handleSetPrice} disabled={!contract}>Set Price</button>
          </div>
        </div>

        <div style={{ border:'1px solid #eee', padding:12, borderRadius:8 }}>
          <h4>Process / Pay</h4>
          <div>
            <button onClick={handleMarkProcessing} disabled={!contract}>Mark Processing</button>
          </div>
          <div style={{ marginTop: 8 }}>
            <button onClick={handleRequestPayment} disabled={!contract}>Request Payment</button>
          </div>
          <div style={{ marginTop: 8 }}>
            <button onClick={handlePay} disabled={!contract}>Pay (reads total)</button>
          </div>
        </div>

        <div style={{ border:'1px solid #eee', padding:12, borderRadius:8 }}>
          <h4>Invoice / Close</h4>
          <div>
            <label>Invoice URI:&nbsp;</label>
            <input value={invoiceUri} onChange={e=>setInvoiceUri(e.target.value)} style={{ width: 300 }} />
          </div>
          <div style={{ marginTop: 8 }}>
            <button onClick={handleUploadInvoice} disabled={!contract}>Upload Invoice</button>
          </div>
          <div style={{ marginTop: 8 }}>
            <button onClick={handleClose} disabled={!contract}>Close & Payout</button>
          </div>
        </div>

        <div style={{ border:'1px solid #eee', padding:12, borderRadius:8 }}>
          <h4>Tracking / Inspect</h4>
          <div>
            <label>Tracking:&nbsp;</label>
            <input value={tracking} onChange={e=>setTracking(e.target.value)} style={{ width: 200 }} />
          </div>
          <div style={{ marginTop: 8 }}>
            <button onClick={handleSetTracking} disabled={!contract}>Set Tracking</button>
          </div>
          <div style={{ marginTop: 8 }}>
            <button onClick={handleLoadOrder} disabled={!contract}>Load Order</button>
          </div>
          {order && (
            <div style={{ marginTop: 8, fontFamily: 'monospace' }}>
              <div>Status: {statusLabels[order.status] ?? order.status}</div>
              <div>FulfillmentFee (wei): {order.fulfillmentFee}</div>
              <div>ShipmentFee (wei): {order.shipmentFee}</div>
              <div>Paid (wei): {order.paid}</div>
              <div>Tracking: {order.tracking || '-'}</div>
              <div>Invoice: {order.invoiceUri || '-'}</div>
            </div>
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
