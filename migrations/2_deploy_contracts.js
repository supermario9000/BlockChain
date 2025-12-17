require('dotenv').config();
const OrderFulfillment = artifacts.require("OrderFulfillment");

module.exports = async function (deployer, network, accounts) {
  const isLocal = network === 'development' || network === 'test';

  let providerFrom;
  let client;
  let courier;

  if (isLocal) {
    // Ganache/Local: use generated accounts
    providerFrom = accounts[0];
    client = accounts[1];
    courier = accounts[2];
  } else {
    // Public nets: prefer provided accounts if hdwallet-provider has multiple keys
    providerFrom = accounts && accounts.length ? accounts[0] : undefined;
    if (accounts && accounts.length >= 3) {
      client = accounts[1];
      courier = accounts[2];
    } else {
      // Fallbacks when only one key is configured
      const maybeClient = process.env.CLIENT_ADDRESS;
      const maybeCourier = process.env.COURIER_ADDRESS;

      const isAddr = (v) => v && web3.utils.isAddress(v);

      client = isAddr(maybeClient) ? maybeClient : providerFrom;
      courier = isAddr(maybeCourier) ? maybeCourier : providerFrom;
      
      if (!client || !courier) {
        throw new Error(
          'CLIENT_ADDRESS and COURIER_ADDRESS must be set in .env when deploying with a single PRIVATE_KEY. ' +
          'Alternatively, set PRIVATE_KEYS with at least 3 comma-separated keys.'
        );
      }
    }
  }

  await deployer.deploy(OrderFulfillment, client, courier, { from: providerFrom });
};
