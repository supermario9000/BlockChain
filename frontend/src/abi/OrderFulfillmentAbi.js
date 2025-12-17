// Minimal ABI required by the frontend
export const abi = [
  { "inputs": [
      { "internalType": "address", "name": "_client", "type": "address" },
      { "internalType": "address", "name": "_courier", "type": "address" }
    ], "stateMutability": "nonpayable", "type": "constructor" },
  { "inputs": [], "name": "client", "outputs": [{"internalType":"address","name":"","type":"address"}], "stateMutability":"view", "type":"function" },
  { "inputs": [], "name": "courier", "outputs": [{"internalType":"address","name":"","type":"address"}], "stateMutability":"view", "type":"function" },
  { "inputs": [], "name": "owner", "outputs": [{"internalType":"address","name":"","type":"address"}], "stateMutability":"view", "type":"function" },
  { "inputs": [], "name": "createOrder", "outputs": [{"internalType":"uint256","name":"orderId","type":"uint256"}], "stateMutability":"nonpayable", "type":"function" },
  { "inputs": [
      {"internalType":"uint256","name":"orderId","type":"uint256"},
      {"internalType":"uint256","name":"fulfillmentFee","type":"uint256"},
      {"internalType":"uint256","name":"shipmentFee","type":"uint256"}
    ], "name":"setPrice","outputs":[],"stateMutability":"nonpayable","type":"function" },
  { "inputs": [{"internalType":"uint256","name":"orderId","type":"uint256"}], "name":"markProcessing","outputs":[],"stateMutability":"nonpayable","type":"function" },
  { "inputs": [{"internalType":"uint256","name":"orderId","type":"uint256"}], "name":"requestPayment","outputs":[],"stateMutability":"nonpayable","type":"function" },
  { "inputs": [{"internalType":"uint256","name":"orderId","type":"uint256"}], "name":"pay","outputs":[],"stateMutability":"payable","type":"function" },
  { "inputs": [
      {"internalType":"uint256","name":"orderId","type":"uint256"},
      {"internalType":"string","name":"uri","type":"string"}
    ], "name":"uploadInvoice","outputs":[],"stateMutability":"nonpayable","type":"function" },
  { "inputs": [{"internalType":"uint256","name":"orderId","type":"uint256"}], "name":"closeAndPayout","outputs":[],"stateMutability":"nonpayable","type":"function" },
  { "inputs": [{"internalType":"uint256","name":"","type":"uint256"}], "name":"orders", "outputs": [
      {"internalType":"uint8","name":"status","type":"uint8"},
      {"internalType":"uint256","name":"fulfillmentFee","type":"uint256"},
      {"internalType":"uint256","name":"shipmentFee","type":"uint256"},
      {"internalType":"uint256","name":"paid","type":"uint256"},
      {"internalType":"string","name":"tracking","type":"string"},
      {"internalType":"string","name":"invoiceUri","type":"string"}
    ], "stateMutability":"view","type":"function" }
];
