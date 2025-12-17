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

While deploying the sepolia environment I ran into a problem with the __sepolia Etherium balance__.  
I tried to get some tokens from free faucets, however most of them also returned that I had insufficient balance.  
These are the ones that didn't work (require mainnet balance or are hard to access):
  * Alchemy faucet   https://www.alchemy.com/faucets/ethereum-sepolia
  * Infura faucet    https://www.infura.io/faucet/sepolia
  * Quicknode faucet https://faucet.quicknode.com/ethereum/sepolia
  * Chainlink faucet https://faucets.chain.link/sepolia

These are the ones that should work:
  * Triangle faucet (worked for me) gives 0.001 sepETH: https://faucet.triangleplatform.com/ethereum/sepolia
  * Google faucet gives 0.005 sepETH  https://cloud.google.com/application/web3/faucet/ethereum/sepolia
  * PoW faucet (requires you to mine the tokens, a large amount available) minimal redeem amount: 0.05 sepETH. https://sepolia-faucet.pk910.de/#/mine/a4674129-e4ad-44fe-9a71-b27aad362162
 
In my case, I needed to get 0.11 sepolia eth tokens. This means only triangle wasn't enough, I used PoW to finish.  

When I had enough balance, my smart contract was deployed and verified by two blocks.  
<img width="856" height="610" alt="image" src="https://github.com/user-attachments/assets/be379170-e66c-48cd-b1ad-7668f8439758" />

Then I proceeded to manually test each step of my smart contract workflow. Images of these tests are below, scaled for formatting:  
|Operation|Result Image|
|---------|------------|
| Create order | <img width="105" height="87" alt="image" src="https://github.com/user-attachments/assets/57e13512-6c0c-4c5b-a7f6-b9e846bd3081" /> |
| Set prices | <img width="122" height="88" alt="image" src="https://github.com/user-attachments/assets/cec6078c-ff62-4b05-b1b4-4133980c1136" /> |
| Mark order as "processing" | <img width="122" height="66" alt="image" src="https://github.com/user-attachments/assets/e2b8cfbf-a119-4106-be8f-5c1fd03c3cc6" /> |
| Request payment from client | <img width="122" height="66" alt="image" src="https://github.com/user-attachments/assets/11480370-00aa-40e6-8a79-4bc9b57f7172" /> |
| Client makes payment (same account in my case)| <img width="122" height="85" alt="image" src="https://github.com/user-attachments/assets/c52b0426-66b5-431a-9c92-fb408185e3b7" /> |
| Upload Invoice | <img width="122" height="84" alt="image" src="https://github.com/user-attachments/assets/7ec16d8d-3447-4533-bdfa-53e56fc90cde" /> |
| Close order and payout | <img width="122" height="88" alt="image" src="https://github.com/user-attachments/assets/6f08818a-3a83-4fa4-819e-c740e0f0b184" /> |
