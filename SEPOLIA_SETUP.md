# Setup Instructions for Sepolia Testnet

## Quick Setup (5 min)

### 1. Get Sepolia RPC Endpoint

**Option A: Infura (Recommended)**
- Go to https://www.infura.io/register
- Create account → Create new project → Select "Ethereum"
- Copy Sepolia RPC URL: `https://sepolia.infura.io/v3/YOUR_API_KEY`

**Option B: Alchemy**
- Go to https://www.alchemy.com
- Create app → Select "Sepolia" network
- Copy HTTPS URL: `https://eth-sepolia.g.alchemy.com/v2/YOUR_KEY`

### 2. Get Your Private Key

1. Open MetaMask wallet
2. Click Account Details → Export Private Key
3. Copy key WITHOUT the `0x` prefix
4. ⚠️ **NEVER** share this publicly!

### 3. Create .env File

Copy `.env.example` to `.env` and fill in:

```bash
cp .env.example .env
```

Edit `.env`:
```
PRIVATE_KEY=your_private_key_without_0x
SEPOLIA_RPC_URL=https://sepolia.infura.io/v3/YOUR_KEY
```

### 4. Get Test ETH

Visit https://www.infura.io/faucet/sepolia and request 0.05 test ETH (takes 1-2 min).

### 5. Deploy Contract

```bash
truffle migrate --network sepolia
```

### 6. Run Tests on Sepolia

```bash
truffle test --network sepolia
```

⚠️ **Important Notes:**
- Each test transaction costs real testnet ETH (but very cheap)
- Tests take ~15-20 sec per transaction (vs instant locally)
- **Recommended**: Only run tests on local network (ganache) during development, then do final testing on Sepolia

### 7. View on Etherscan

Go to https://sepolia.etherscan.io and search for your contract address to see all transactions.

## Running Tests

**Local (Ganache) - FAST & FREE:**
```bash
# Start ganache in one terminal
ganache-cli --port 8545 --chainId 1337

# In another terminal, run tests
npx truffle test --network development
```

**Testnet (Sepolia) - SLOW & COSTS ETH:**
```bash
npx truffle test --network sepolia
```

The **same test file** (`test/OrderFulfillment.js`) runs on both networks—you just change the `--network` flag!
