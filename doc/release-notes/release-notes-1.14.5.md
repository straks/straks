
STRAKS 1.14.5 Release Notes: First Release
==========================================

### Specifications

#### Coinbase Transactions, Pool Mining and Wallet Mining

**IMPORTANT**

Due to the unique combination of segwit, treasury reward, miner reward and masternode payments, as of this writing, mining software and pools do not yet support STRAKS' coinbasetxn configuration.  The only way to mine at the moment is via STRAKS' wallet, as follows:

```
Approach 1 (Local):
(1) Start straksd/straks-qt locally
(2) Run: "./straks-cli generate 1" (or via debug console in the Qt gui wallet)
```
```
Approach 2 (Remote):
(1) Start straksd on remote server and local server
(2) On your local wallet (server) run: "./straks-cli getaccountaddress 0"
(3) On Your remote server Run: "./straks-cli generatetoaddress 1 <address from step2>"
(4) Shutdown local wallet (server)
```
We find ourselves in an attractive position of having a low barrier to entry at launch, which allows anyone and everyone to start mining from the outset.  You are only limited by the number of wallets that can be run simultaneously.  The latter is comparatively cheaper than setting up a multi-gpu mining rig.

We are curently working with mining pools to ensure that we can enable GPU/CPU pool mining as soon as practicable.


#### Zero Start Instamine Protection (ZSIP)

STRAKS blockchain has been set to issue a 0 block subsidy up to **block 10080**.  At 1440 blocks per day, this is approximately 7 days. During this time, community members will be informed of launch and will be able to configure their setups for mining.  During this period there are no Treasury payments, either.


#### Maximum Coin Supply and Block Reward

STRAKS will be capped at a maxium supply of **150 million coins**.  This target supports a managed inflation rate that allows for a constant block reward of **10 STAKs**.

The constant block reward prevents event cliffs related to expected "halvings" and will support a more consistent and stable participation rate over a long time horizon.

More information will be available on the STRAKS [website](http://straks.io)

##### The Monetary Curve Based on a 10 STAK Block Reward
![Alt text](mcurve.png)


##### The Monetary Curve Underlying Dataset with Zero Start Block Offset
![Alt text](monetary_curve_table.png)


#### POW Algorithm: Lyra2REv2

Fast, power-efficient and asic resistant algorithm.  The STRAKS development team will continue to support a decentralized asic resistant blockchain, and will update the POW algorithm if the latter is no longer the case.

The difference between Lyra2RE and Lyra2REv2 is as follows;

- Addition of 2 rounds of CubeHash (https://en.wikipedia.org/wiki/CubeHash)
- Addition of 1 round of Blue Midnight Wish (https://www.mathematik.hu-berlin.de/~schliebn/dl/Blue-Midnight-Wish.pdf)
- Removal of Groestl (http://www.groestl.info/)

More specific information about Lyra2 itself can be obtained from: http://lyra-2.net/


#### **NEW** Difficulty Retargeting Algorithm (D106 - Amaury Sechet)

Recent public discussions on how to address Bitcoin Cash's hash rate oscillation has resulted in an excellent approach that addresses many weaknesses of various other algorithms.  A well developed analsysis was presented by Bitcoin Cash's developer Amaury Sechet and supported by Zawy12 via thorough analysis, with simulated results that demonstate the algorthim's effectiveness.  The proposed implementation was modified specific to STRAKS blockchain.

More information can be found here:

- https://github.com/seredat/karbowanec/commit/231db5270acb2e673a641a1800be910ce345668a#commitcomment-22615466
- https://lists.linuxfoundation.org/pipermail/bitcoin-ml/2017-August/000136.html
- https://news.bitcoin.com/bitcoin-cash-hard-fork-plans-updated-new-difficulty-adjustment-algorithm-chosen/

##### The Modified D106 Algorithm Targeting a 60 Second Block Time for STRAKS Blockchain
![Alt text](D106_algorithm_test.png)


#### Treasury Funding

In order to support continued development, exchange lisiting fees, web/node hosting costs, and various other operational costs related to running a successful blockchain, the STRAKS development team have decided to introduce a 5% treasury fee in perpetuity. We intend to ensure that the new currency is competitive and successful. To be able to deliver on that vision, we need to ensure that the currency is well-funded throughout its lifecycle.

At 5%, approximately 72 blocks per day are mined for treasury (720 STAK).  In the spirit
of transparency, the following 2-of-3 multisig addresses are the official
treasury addresses:

```
[0] 3K3bPrW5h7DYEMp2RcXawTCXajcm4ZU9Zh
[1] 33Ssxmn3ehVMgyxgegXhpLGSBpubPjLZQ6
[2] 3HFPNAjesiBY5sSVUmuBFnMEGut69R49ca
[3] 37jLjjfUXQU4bdqVzvpUXyzAqPQSmxyByi

```

#### Masternodes

Masternodes will be supported with a collateral requirement of **15,000 STAK**.  This
is a dynamic target value and will be updated in future releases based on the
market price of STRAKS and return on investment relative to its peer group.

Masternode payments do not start until after approximately 5 weeks post launch. More
specifically, after **block number 50400**.  This is an active promotion of masternode
decentralisation.  The time-delay seeks to provide  sufficent time for swap holders and
ivnestors to set up nodes.


#### Masternode Payment Rebalancing: **NEW** Reactive Equilibria (REV1)

Unique to STRAKS is the use of a new continous activation function based algorithm,
labelled Reactive Equilibria (REV1), that seeks to rebalance the payments made
to miners versus masternodes.  There is a hard limit of a maxmium 60% that
can be appropriated for masternode payments, but that would assume that the
total supply locked by masternodes versus coin circulation is below ~13%.
As the masternode count increases, the payment to masternode holders will
decline in favour of miners in order to support a decentralised ideology.  The
new algorithm is simple in design but highly effective and lays the foundation for
future advancements.  More information abut this feature will be posted on the
[main site](http://straks.io).

##### Reactive Equilibria Masternode Payment Rebalancing Profile
![Alt text](reactive_equilibria.png)


#### **NEW** Segregated Witness 2x/4x

![Alt text](segwit.png)

SegWit2x is a combination of both SegWit and a 2MB hardfork. STRAKS has further
increased the non-segwit block size to 4MB from the outset to allow for greater
scalability and utility.  The maximum serialised block size is 16MB for STRAKS.

STRAKS is both lightning network and atomic swap compatible. And unlike
many other "compatible" altcoins, SegWit, SegWit2x and the larger blocksize are enabled from the outset.

For further information about Segregated Witness:

https://bitcoincore.org/en/2016/01/26/segwit-benefits/

##### Segregated Witness Transaction on STRAKS Testnet, [block:6635, txid:3fc630ac1a4b91714d7c7150275b8be019152329e70d1f7a37240a7331b9fab6]
![Alt text](testnet_segwit.png)


#### InstantTX and DarkSend Removal
*(Reproduced here for information purposes only, not release related)*

Dropped support for both InstantTX and DarkSend. With the SegWit upgrade these features are going to be superseeded by far superior technology. SegWit will enable the STRAKS to adopt the Lightning Network (https://lightning.network/lightning-network-paper.pdf), cross-chain atomic swaps, advanced versions of TumbleBit (https://eprint.iacr.org/2016/575.pdf) and more.

#### Hierarchical Deterministic Key Generation
*(Reproduced here for information purposes only, not release related)*

Newly created wallets will use hierarchical deterministic key generation
according to BIP32 (keypath m/0'/0'/k').
Existing wallets will still use traditional key generation.

Backups of HD wallets, regardless of when they have been created, can
therefore be used to re-generate all possible private keys, even the
ones which haven't already been generated during the time of the backup.
**Attention:** Encrypting the wallet will create a new seed which requires
a new backup!

Wallet dumps (created using the `dumpwallet` RPC) will contain the deterministic
seed. This is expected to allow future versions to import the seed and all
associated funds, but this is not yet implemented.

HD key generation for new wallets can be disabled by `-usehd=0`. Keep in
mind that this flag only has affect on newly created wallets.
You can't disable HD key generation once you have created a HD wallet.

There is no distinction between internal (change) and external keys.

HD wallets are incompatible with older versions of Bitcoin Core.

[Pull request](https://github.com/bitcoin/bitcoin/pull/8035/files), [BIP 32](https://github.com/bitcoin/bips/blob/master/bip-0032.mediawiki)


#### Signature validation using libsecp256k1 [Noteworthy: pre-STRAKS]
*(Reproduced here for information purposes only, not release related)*

ECDSA signatures inside Bitcoin transactions now use validation using
[libsecp256k1](https://github.com/bitcoin-core/secp256k1) instead of OpenSSL.

Depending on the platform, this means a significant speedup for raw signature
validation speed. The advantage is largest on x86_64, where validation is over
five times faster. In practice, this translates to a raw reindexing and new
block validation times that are less than half of what it was before.

Libsecp256k1 has undergone very extensive testing and validation.

A side effect of this change is that libconsensus no longer depends on OpenSSL.

##### Direct headers announcement (BIP 130) [Noteworthy: pre-STRAKS]
*(Reproduced here for information purposes only, not release related)*

Between compatible peers, [BIP 130]
(https://github.com/bitcoin/bips/blob/master/bip-0130.mediawiki)
direct headers announcement is used. This means that blocks are advertised by
announcing their headers directly, instead of just announcing the hash. In a
reorganization, all new headers are sent, instead of just the new tip. This
can often prevent an extra roundtrip before the actual block is downloaded.


__________________________________________________________________________


### Acknowledgements

Credit goes to Bitcoin Core, Dash and Bitsend for providing a basic platform for
STRAKS to enhance and develop, in concert with a shared desire to support the
adoption of a decentralised digital currency future for the masses.


__________________________________________________________________________


### License

STRAKS Core is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.


__________________________________________________________________________


### Development Process

The `master` branch is meant to be stable. Development is normally done in separate branches.
[Tags](https://github.com/straks/straks/tags) are created to indicate new official,
stable release versions of STRAKS Core.

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md).


__________________________________________________________________________


### Building process

**compiling straks from git**

Use the autogen script to prepare the build environment.

    ./autogen.sh
    ./configure
    make

**precompiled binaries**

Precompiled binaries are available at GitHub, see
https://github.com/straks/straks/releases

Always verify the signatures and checksums.


__________________________________________________________________________


### Development tips and tricks

**compiling for debugging**

Run configure with the --enable-debug option, then make. Or run configure with
CXXFLAGS="-g -ggdb -O0" or whatever debug flags you need.

**debug.log**

If the code is behaving strangely, take a look in the debug.log file in the data directory;
error and debugging message are written there.

The -debug=... command-line option controls debugging; running with just -debug will turn
on all categories (and give you a very large debug.log file).

The Qt code routes qDebug() output to debug.log under category "qt": run with -debug=qt
to see it.

**testnet and regtest modes**

Run with the -testnet option to run with "play strakss" on the test network, if you
are testing multi-machine code that needs to operate across the internet.

If you are testing something that can run on one machine, run with the -regtest option.
In regression test mode blocks can be created on-demand; see qa/rpc-tests/ for tests
that run in -regtest mode.

**DEBUG_LOCKORDER**

STRAKS Core is a multithreaded application, and deadlocks or other multithreading bugs
can be very difficult to track down. Compiling with -DDEBUG_LOCKORDER (configure
CXXFLAGS="-DDEBUG_LOCKORDER -g") inserts run-time checks to keep track of what locks
are held, and adds warning to the debug.log file if inconsistencies are detected.
