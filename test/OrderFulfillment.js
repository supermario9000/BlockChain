const OrderFulfillment = artifacts.require("OrderFulfillment");

contract("OrderFulfillment", (accounts) => {
  const [provider, client, courier, other] = accounts;
  let contract;

  // Prieš kiekvieną testą sukuriamas naujas sutarties egzempliorius
  beforeEach(async () => {
    contract = await OrderFulfillment.new(client, courier, { from: provider });
  });

  describe("Deployment", () => {
    // Testas tikrina, ar teisingai priskirti kliento ir kurjerio adresai sutartyje
    it("should set the correct client and courier addresses", async () => {
      const storedClient = await contract.client();
      const storedCourier = await contract.courier();
      assert.equal(storedClient, client, "Client address mismatch");
      assert.equal(storedCourier, courier, "Courier address mismatch");
    });

    // Testas tikrina, ar 3PL paslaugų teikėjas (provider) yra sutarties savininkas
    it("should set the provider as owner", async () => {
      const owner = await contract.owner();
      assert.equal(owner, provider, "Owner should be provider");
    });
  });

  describe("Order Creation", () => {
    // Testas tikrina, ar paslaugų teikėjas gali sukurti užsakymą ir ar jis gauna teisingą ID
    it("should allow provider to create an order", async () => {
      const tx = await contract.createOrder({ from: provider });
      const orderId = tx.logs[0].args.orderId.toNumber();
      
      assert.equal(orderId, 0, "First order ID should be 0");
      
      const order = await contract.orders(orderId);
      assert.equal(order.status.toNumber(), 0, "Status should be Registered");
    });

    // Testas tikrina, kad ne savininkas negali sukurti užsakymo
    it("should not allow non-provider to create an order", async () => {
      try {
        await contract.createOrder({ from: other });
        assert.fail("Should have thrown an error");
      } catch (error) {
        assert.include(error.message, "revert", "Should revert transaction");
      }
    });

    // Testas tikrina, ar užsakymų ID didėja teisingai (0, 1, 2...)
    it("should increment order IDs correctly", async () => {
      await contract.createOrder({ from: provider });
      const tx = await contract.createOrder({ from: provider });
      const orderId = tx.logs[0].args.orderId.toNumber();
      
      assert.equal(orderId, 1, "Second order ID should be 1");
    });
  });

  describe("Price Setting", () => {
    let orderId;

    beforeEach(async () => {
      const tx = await contract.createOrder({ from: provider });
      orderId = tx.logs[0].args.orderId.toNumber();
    });

    // Testas tikrina, ar paslaugų teikėjas gali nustatyti kainas registruotam užsakymui
    it("should allow provider to set price for registered order", async () => {
      const fulfillmentFee = web3.utils.toWei("0.5", "ether");
      const shipmentFee = web3.utils.toWei("0.3", "ether");

      const tx = await contract.setPrice(orderId, fulfillmentFee, shipmentFee, { from: provider });
      
      assert.equal(tx.logs[0].event, "PriceSet", "Should emit PriceSet event");
      assert.equal(tx.logs[1].event, "StatusChanged", "Should emit StatusChanged event");

      const order = await contract.orders(orderId);
      assert.equal(order.fulfillmentFee.toString(), fulfillmentFee, "Fulfillment fee mismatch");
      assert.equal(order.shipmentFee.toString(), shipmentFee, "Shipment fee mismatch");
      assert.equal(order.status.toNumber(), 1, "Status should be Priced");
    });

    // Testas tikrina, kad tik savininkas gali nustatyti kainas
    it("should not allow non-provider to set price", async () => {
      try {
        await contract.setPrice(orderId, 1000, 500, { from: other });
        assert.fail("Should have thrown an error");
      } catch (error) {
        assert.include(error.message, "revert", "Should revert transaction");
      }
    });
  });

  describe("Full Order Flow", () => {
    // Testas tikrina visą užsakymo ciklą: sukūrimas → kainos nustatymas → apdorojimas → mokėjimas → sąskaita → uždarymas
    it("should complete a full order lifecycle successfully", async () => {
      const fulfillmentFee = web3.utils.toWei("0.5", "ether");
      const shipmentFee = web3.utils.toWei("0.3", "ether");
      const totalFee = web3.utils.toBN(fulfillmentFee).add(web3.utils.toBN(shipmentFee));

      // Step 1: Create order
      const createTx = await contract.createOrder({ from: provider });
      const orderId = createTx.logs[0].args.orderId.toNumber();

      // Step 2: Set price
      await contract.setPrice(orderId, fulfillmentFee, shipmentFee, { from: provider });

      // Step 3: Mark as processing
      await contract.markProcessing(orderId, { from: provider });
      let order = await contract.orders(orderId);
      assert.equal(order.status.toNumber(), 2, "Status should be Processing");

      // Step 4: Request payment
      await contract.requestPayment(orderId, { from: provider });
      order = await contract.orders(orderId);
      assert.equal(order.status.toNumber(), 3, "Status should be AwaitingPayment");

      // Step 5: Client pays
      const clientBalanceBefore = await web3.eth.getBalance(client);
      const payTx = await contract.pay(orderId, { from: client, value: totalFee });
      order = await contract.orders(orderId);
      assert.equal(order.status.toNumber(), 4, "Status should be Paid");
      assert.equal(order.paid.toString(), totalFee.toString(), "Paid amount mismatch");

      // Step 6: Upload invoice
      const invoiceUri = "ipfs://QmXxxx";
      await contract.uploadInvoice(orderId, invoiceUri, { from: provider });
      order = await contract.orders(orderId);
      assert.equal(order.status.toNumber(), 5, "Status should be Invoiced");
      assert.equal(order.invoiceUri, invoiceUri, "Invoice URI mismatch");

      // Step 7: Close and payout
      const providerBalanceBefore = await web3.eth.getBalance(provider);
      const courierBalanceBefore = await web3.eth.getBalance(courier);

      const closeTx = await contract.closeAndPayout(orderId, { from: provider });
      
      order = await contract.orders(orderId);
      assert.equal(order.status.toNumber(), 6, "Status should be Closed");

      // Check payouts (accounting for gas costs)
      const courierBalanceAfter = await web3.eth.getBalance(courier);
      const courierIncrease = web3.utils.toBN(courierBalanceAfter).sub(web3.utils.toBN(courierBalanceBefore));
      assert.equal(courierIncrease.toString(), shipmentFee, "Courier should receive shipment fee");
    });
  });

  describe("Payment Validation", () => {
    let orderId;
    const fulfillmentFee = web3.utils.toWei("0.5", "ether");
    const shipmentFee = web3.utils.toWei("0.3", "ether");
    const totalFee = web3.utils.toBN(fulfillmentFee).add(web3.utils.toBN(shipmentFee));

    beforeEach(async () => {
      const tx = await contract.createOrder({ from: provider });
      orderId = tx.logs[0].args.orderId.toNumber();
      await contract.setPrice(orderId, fulfillmentFee, shipmentFee, { from: provider });
      await contract.markProcessing(orderId, { from: provider });
      await contract.requestPayment(orderId, { from: provider });
    });

    // Testas tikrina, kad mokėjimas su neteisinga suma yra atmestas
    it("should reject payment with incorrect amount", async () => {
      const wrongAmount = web3.utils.toWei("0.5", "ether");
      try {
        await contract.pay(orderId, { from: client, value: wrongAmount });
        assert.fail("Should have thrown an error");
      } catch (error) {
        assert.include(error.message, "Incorrect amount");
      }
    });

    // Testas tikrina, kad tik klientas gali atlikti mokėjimą
    it("should reject payment from non-client", async () => {
      try {
        await contract.pay(orderId, { from: other, value: totalFee });
        assert.fail("Should have thrown an error");
      } catch (error) {
        assert.include(error.message, "Not client");
      }
    });

    // Testas tikrina, kad mokėti galima tik kai užsakymo būsena yra 'AwaitingPayment'
    it("should only allow payment in AwaitingPayment status", async () => {
      // Pay once successfully
      await contract.pay(orderId, { from: client, value: totalFee });
      
      // Try to pay again
      try {
        await contract.pay(orderId, { from: client, value: totalFee });
        assert.fail("Should have thrown an error");
      } catch (error) {
        assert.include(error.message, "Bad status");
      }
    });
  });

  describe("Status Transitions", () => {
    let orderId;

    beforeEach(async () => {
      const tx = await contract.createOrder({ from: provider });
      orderId = tx.logs[0].args.orderId.toNumber();
    });

    // Testas tikrina, kad negalima pažymėti užsakymo kaip apdorojamo be kainos nustatymo
    it("should not allow marking processing without pricing", async () => {
      try {
        await contract.markProcessing(orderId, { from: provider });
        assert.fail("Should have thrown an error");
      } catch (error) {
        assert.include(error.message, "Bad status");
      }
    });

    // Testas tikrina, kad negalima prašyti mokėjimo be užsakymo apdorojimo žymėjimo
    it("should not allow requesting payment without marking processing", async () => {
      await contract.setPrice(orderId, 1000, 500, { from: provider });
      
      try {
        await contract.requestPayment(orderId, { from: provider });
        assert.fail("Should have thrown an error");
      } catch (error) {
        assert.include(error.message, "Bad status");
      }
    });
  });

  describe("Order Cancellation", () => {
    let orderId;

    beforeEach(async () => {
      const tx = await contract.createOrder({ from: provider });
      orderId = tx.logs[0].args.orderId.toNumber();
    });

    // Testas tikrina, ar paslaugų teikėjas gali atšaukti registruotą užsakymą
    it("should allow provider to cancel registered order", async () => {
      await contract.cancel(orderId, { from: provider });
      const order = await contract.orders(orderId);
      assert.equal(order.status.toNumber(), 7, "Status should be Cancelled");
    });

    // Testas tikrina, ar paslaugų teikėjas gali atšaukti užsakymą su nustatyta kaina
    it("should allow provider to cancel priced order", async () => {
      await contract.setPrice(orderId, 1000, 500, { from: provider });
      await contract.cancel(orderId, { from: provider });
      const order = await contract.orders(orderId);
      assert.equal(order.status.toNumber(), 7, "Status should be Cancelled");
    });

    // Testas tikrina, kad apmokėto užsakymo atšaukti negalima
    it("should not allow cancellation of paid order", async () => {
      await contract.setPrice(orderId, 1000, 500, { from: provider });
      await contract.markProcessing(orderId, { from: provider });
      await contract.requestPayment(orderId, { from: provider });
      await contract.pay(orderId, { from: client, value: 1500 });

      try {
        await contract.cancel(orderId, { from: provider });
        assert.fail("Should have thrown an error");
      } catch (error) {
        assert.include(error.message, "Bad status");
      }
    });
  });

  describe("Tracking Information", () => {
    let orderId;

    beforeEach(async () => {
      const tx = await contract.createOrder({ from: provider });
      orderId = tx.logs[0].args.orderId.toNumber();
      await contract.setPrice(orderId, 1000, 500, { from: provider });
      await contract.markProcessing(orderId, { from: provider });
      await contract.requestPayment(orderId, { from: provider });
      await contract.pay(orderId, { from: client, value: 1500 });
    });

    // Testas tikrina, ar paslaugų teikėjas gali pridėti siuntimo sekimo kodą
    it("should allow provider to set tracking information", async () => {
      const trackingCode = "TRACK123456";
      await contract.setTracking(orderId, trackingCode, { from: provider });
      
      const order = await contract.orders(orderId);
      assert.equal(order.tracking, trackingCode, "Tracking code mismatch");
    });

    // Testas tikrina, kad tik paslaugų teikėjas gali nustatyti sekimo informaciją
    it("should not allow non-provider to set tracking", async () => {
      try {
        await contract.setTracking(orderId, "TRACK123", { from: other });
        assert.fail("Should have thrown an error");
      } catch (error) {
        assert.include(error.message, "revert", "Should revert transaction");
      }
    });
  });
});
