const OrderFulfillment = artifacts.require("OrderFulfillment");

module.exports = async function (deployer, network, accounts) {
  // For local development we use the first accounts as provider, client, courier
  // Adjust these addresses for testnet/mainnet deployments.
  const provider = accounts[0];
  const client = accounts[1];
  const courier = accounts[2];

  await deployer.deploy(OrderFulfillment, client, courier, { from: provider });
};
