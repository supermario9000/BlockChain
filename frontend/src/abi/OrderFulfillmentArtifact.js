// Import the compiled contract artifact from truffle build
import artifact from '../../../build/contracts/OrderFulfillment.json'

export const CONTRACT_BYTECODE = artifact.bytecode
export const CONTRACT_ABI = artifact.abi
