# Ethereum based Smart Contract implementation

_For Lithuanian documentation, look below._

## Scenario and business model explanation

As I have some experience working in a 3PL logistics fulfillment centre, I chose such company as a template for this task.  

__3PL__ - A service for Eshops to not only store their products in a warehouse, but fulfill customer's orders directly from the warehouse when an order is placed.  
This means that order processing is done by the 3PL service provider and the owner of an Eshop doesn't need any input in the process.  

### Scenario

The scenario is as follows: A 3PL logistics company has many clients, who store their products in their warehouse.  
Each client can have different products, and because of that each order fulfillment price can be different.  
When each order is being registered, there should be a smart contract involved, to ensure fast payments for the logistics company and steady order updates for the customer.  
This makes the smart contract process a B2B transaction for a service, as the logistics company is selling their "product" as a service.  

### Business model
This is the logic model I came up with.  
It is similar to the example model (https://medium.com/coinmonks/build-a-smart-contract-to-sell-goods-6cf73609d25), however i reworked it to fit my scenario.  

Below you will find an image which shows the sequence diagram of my business model and a step-by step explanation of all elements.  

The picture shows 4 parties: the 3PL logistics company, the customer, the smart contract and the courier.  
Each parties can and usually do have their own system, so even events and conversations not between the smart contract and the subject are shown.  

<img width="600" height="920" alt="Untitled Diagram drawio (1)" src="https://github.com/user-attachments/assets/5762b111-d69a-4377-9d2b-57e3656d931c" />

A step by step explanation:
1. The customer sends their products to the 3PL service provider's warehouse. What the 3PL logistics company does with the received inventory doesn't really matter in this case, so _let's assume they put everything neatly into shelves and ready the products for fulfillment_ (this can include labeling etc.)  
2. The 3PL logistics company initiates a smart contract exclusively for their customer's account.
3. The 3PL logistics company always keeps an updated courier pricelist, so to do that they might ask the courier to provide it.
4. The customer registers an order on the 3PL logistics system. The order usually can always be registered on the WMS (warehouse management system), but it can be allowed for fulfillment only if there is stock available.
5. This becomes the next step, as the 3PL logistics company has to check their system for stock (usually done automatically) and officially register the order (update the status in the smart contract). The updated status then can be viewed by the customer.
6. After the order is ready for processing, the logistics company sends the fulfillment (this takes into account the amount of goods in one order, the total weight etc. It can be technically based on any agreed condition) and shipment prices to the smart contract. The client can easily view the updated prices and prepare money for payment.
7. Then the company processes the order, this is done without working with any other parties. The WMS automatically updates the status in the smart contract. The client may set up a system to get updates about their orders' status.
8. The client proceeds with the payment to the smart contract. The payment is done safely as to not lose any money on the way or not to breach any security norms.
9. The logistics company gives the packaged order to the courier and receives a tracking code in return. This information is available only to the 3PL company in my case, as it is a message received in return. It could also be updated in the smart contract, but I deemed it unnecessary.
10. Then the logistics company uploads an invoice for their service in the smart contract. The client gets the update. 
11. After the delivery has been finished, the smart contract stores the order status as closed, payouts to the parties which performed the service: 3PL logistics company gets the fulfillment payment, courier gets the shipment payment.
12. The status is updated to the client, and the client receives a stock decrease report from the 3PL logistics WMS.

I hope everything is understandable up to this point.

# Tests and deployment

## Localhost testing
As I was not sure of what tests i should perform, i did a simple full business model loop test check.  
After troubleshooting for a while, everything returned the right statuses and worked correctly.  
The picture below shows what functions of the loop I was checking and their return times.  
<img width="558" height="515" alt="image" src="https://github.com/user-attachments/assets/357edf2e-27e8-4c6a-99b6-fd56f2033564" />  

Testing was done using ganache-cli, which created a local ethereum network.  
That means that each transaction occurs on different blocks, which leads to very sharp performance and usually this test only takes 5 seconds.  

## Sepolia testing (testing in a net-wide sandbox environment)
For this section i performed the same tests.  
