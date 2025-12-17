// SPDX-License-Identifier: MIT
pragma solidity ^0.8.20;

import "@openzeppelin/contracts/access/Ownable.sol";
import "@openzeppelin/contracts/utils/ReentrancyGuard.sol";

contract OrderFulfillment is Ownable, ReentrancyGuard {
    enum Status { Registered, Priced, Processing, AwaitingPayment, Paid, Invoiced, Closed, Cancelled }

    struct Order {
        Status status;
        uint256 fulfillmentFee;
        uint256 shipmentFee;
        uint256 paid;
        string tracking;
        string invoiceUri;
    }

    address public courier;
    address public client;
    uint256 public nextOrderId;
    mapping(uint256 => Order) public orders;

    event OrderCreated(uint256 indexed orderId);
    event PriceSet(uint256 indexed orderId, uint256 fulfillmentFee, uint256 shipmentFee);
    event StatusChanged(uint256 indexed orderId, Status status);
    event Paid(uint256 indexed orderId, uint256 amount);
    event InvoiceUploaded(uint256 indexed orderId, string uri);
    event PayoutsReleased(uint256 indexed orderId, uint256 toProvider, uint256 toCourier);

    modifier onlyProvider() { _checkOwner(); _; }
    modifier onlyClient() { require(msg.sender == client, "Not client"); _; }

    constructor(address _client, address _courier) Ownable(msg.sender) {
        client = _client;
        courier = _courier;
    }

    function createOrder() external onlyProvider returns (uint256 orderId) {
        orderId = nextOrderId++;
        orders[orderId].status = Status.Registered;
        emit OrderCreated(orderId);
    }

    function setPrice(uint256 orderId, uint256 fulfillmentFee, uint256 shipmentFee) external onlyProvider {
        Order storage o = orders[orderId];
        require(o.status == Status.Registered, "Bad status");
        o.fulfillmentFee = fulfillmentFee;
        o.shipmentFee = shipmentFee;
        o.status = Status.Priced;
        emit PriceSet(orderId, fulfillmentFee, shipmentFee);
        emit StatusChanged(orderId, Status.Priced);
    }

    function markProcessing(uint256 orderId) external onlyProvider {
        Order storage o = orders[orderId];
        require(o.status == Status.Priced, "Bad status");
        o.status = Status.Processing;
        emit StatusChanged(orderId, Status.Processing);
    }

    function requestPayment(uint256 orderId) external onlyProvider {
        Order storage o = orders[orderId];
        require(o.status == Status.Processing, "Bad status");
        o.status = Status.AwaitingPayment;
        emit StatusChanged(orderId, Status.AwaitingPayment);
    }

    function pay(uint256 orderId) external payable onlyClient nonReentrant {
        Order storage o = orders[orderId];
        require(o.status == Status.AwaitingPayment, "Bad status");
        uint256 due = o.fulfillmentFee + o.shipmentFee;
        require(msg.value == due, "Incorrect amount");
        o.paid = msg.value;
        o.status = Status.Paid;
        emit Paid(orderId, msg.value);
        emit StatusChanged(orderId, Status.Paid);
    }

    function setTracking(uint256 orderId, string calldata tracking) external onlyProvider {
        Order storage o = orders[orderId];
        require(o.status == Status.Paid || o.status == Status.AwaitingPayment || o.status == Status.Processing, "Bad status");
        o.tracking = tracking;
    }

    function uploadInvoice(uint256 orderId, string calldata uri) external onlyProvider {
        Order storage o = orders[orderId];
        require(o.status == Status.Paid, "Bad status");
        o.invoiceUri = uri;
        o.status = Status.Invoiced;
        emit InvoiceUploaded(orderId, uri);
        emit StatusChanged(orderId, Status.Invoiced);
    }

    function closeAndPayout(uint256 orderId) external onlyProvider nonReentrant {
        Order storage o = orders[orderId];
        require(o.status == Status.Invoiced, "Bad status");
        o.status = Status.Closed;
        uint256 toCourier = o.shipmentFee;
        uint256 toProvider = o.paid - toCourier;
        emit StatusChanged(orderId, Status.Closed);
        emit PayoutsReleased(orderId, toProvider, toCourier);
        (bool ok1, ) = owner().call{value: toProvider}("");
        require(ok1, "Provider payout failed");
        (bool ok2, ) = courier.call{value: toCourier}("");
        require(ok2, "Courier payout failed");
    }

    function cancel(uint256 orderId) external onlyProvider {
        Order storage o = orders[orderId];
        require(o.status == Status.Registered || o.status == Status.Priced || o.status == Status.Processing, "Bad status");
        o.status = Status.Cancelled;
        emit StatusChanged(orderId, Status.Cancelled);
    }
}