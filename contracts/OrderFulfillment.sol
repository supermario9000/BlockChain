// SPDX-License-Identifier: MIT
pragma solidity ^0.8.20;

// OpenZeppelin imports: Ownable provides owner-only access control, ReentrancyGuard prevents reentrancy attacks
import "@openzeppelin/contracts/access/Ownable.sol";
import "@openzeppelin/contracts/utils/ReentrancyGuard.sol";

contract OrderFulfillment is Ownable, ReentrancyGuard {
    // Enum defines all possible order states in the workflow
    enum Status { Registered, Priced, Processing, AwaitingPayment, Paid, Invoiced, Closed, Cancelled }

    // Struct holds all data for a single order
    struct Order {
        Status status;              // Current order state
        uint256 fulfillmentFee;     // Fee for 3PL warehouse processing (in wei)
        uint256 shipmentFee;        // Fee for courier delivery (in wei)
        uint256 paid;               // Total amount paid by client (in wei)
        string tracking;            // Courier tracking code
        string invoiceUri;          // URI/hash of invoice document (IPFS, etc.)
    }

    // State variables store addresses and order data
    address public courier;         // Address that receives shipment fees
    address public client;          // Address authorized to pay for orders
    uint256 public nextOrderId;     // Counter for generating unique order IDs
    mapping(uint256 => Order) public orders;  // Maps order ID to Order struct (accessible by anyone)

    // Events are emitted to log important actions on the blockchain (indexed params are searchable)
    event OrderCreated(uint256 indexed orderId);
    event PriceSet(uint256 indexed orderId, uint256 fulfillmentFee, uint256 shipmentFee);
    event StatusChanged(uint256 indexed orderId, Status status);
    event Paid(uint256 indexed orderId, uint256 amount);
    event InvoiceUploaded(uint256 indexed orderId, string uri);
    event PayoutsReleased(uint256 indexed orderId, uint256 toProvider, uint256 toCourier);

    // Modifiers add preconditions to functions
    modifier onlyProvider() { _checkOwner(); _; }  // Only owner (provider) can call
    modifier onlyClient() { require(msg.sender == client, "Not client"); _; }  // Only client can call

    constructor(address _client, address _courier) Ownable(msg.sender) {
        client = _client;
        courier = _courier;
    }

    function createOrder() external onlyProvider returns (uint256 orderId) {
        orderId = nextOrderId++;  // Assign current ID, then increment for next order
        orders[orderId].status = Status.Registered;  // Initialize with first status
        emit OrderCreated(orderId);
    }

    function setPrice(uint256 orderId, uint256 fulfillmentFee, uint256 shipmentFee) external onlyProvider {
        Order storage o = orders[orderId];  // 'storage' means we modify the actual stored data
        require(o.status == Status.Registered, "Bad status");  // Enforce status workflow
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
        uint256 due = o.fulfillmentFee + o.shipmentFee;  // Calculate total
        require(msg.value == due, "Incorrect amount");    // Exact payment required
        o.paid = msg.value;  // Record payment amount
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
        
        // Update state first (prevents reentrancy exploitation)
        o.status = Status.Closed;
        uint256 toCourier = o.shipmentFee;
        uint256 toProvider = o.paid - toCourier;  // Provider gets fulfillment fee
        
        emit StatusChanged(orderId, Status.Closed);
        emit PayoutsReleased(orderId, toProvider, toCourier);
        
        // Transfer ETH to provider (owner)
        (bool ok1, ) = owner().call{value: toProvider}("");
        require(ok1, "Provider payout failed");
        
        // Transfer ETH to courier
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