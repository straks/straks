#include "signhelper_mn.h"
#include "masternodeman.h"
#include "masternode.h"

/*
Global
*/
int RequestedMasterNodeList = 0;


void ThreadBitPool()
{
    if(fProUserModeDarksendInstantX2) return; //disable all Darksend/Masternode related functionality

    // Make this thread recognisable as the wallet flushing thread
    RenameThread("straks-darksend");

    unsigned int c = 0;
    std::string errorMessage;

    while (true)
    {
        c++;

        MilliSleep(1000);
        //LogPrintf("ThreadCheckDarkSendPool::check timeout\n");

        //darkSendPool.CheckTimeout();
        //darkSendPool.CheckForCompleteQueue();

        if(c % 60 == 0)
        {
            LOCK(cs_main);
            /*
                cs_main is required for doing CMasternode.Check because something
                is modifying the coins view without a mempool lock. It causes
                segfaults from this code without the cs_main lock.
            */
            mnodeman.CheckAndRemove();
            mnodeman.ProcessMasternodeConnections();
            masternodePayments.CleanPaymentList();
            //CleanTransactionLocksList();
        }

        if(c % MASTERNODE_PING_SECONDS == 0) activeMasternode.ManageStatus();

        if(c % MASTERNODES_DUMP_SECONDS == 0) DumpMasternodes();

        //try to sync the Masternode list and payment list every 5 seconds from at least 3 nodes
        if(c % 5 == 0 && RequestedMasterNodeList < 3){
            bool fIsInitialDownload = IsInitialBlockDownload();
            if(!fIsInitialDownload) {
                //LOCK(cs_vNodes);
                //BOOST_FOREACH(CNode* pnode, vNodes)
				g_connman->ForEachNode([](CNode* pnode)
                {
                    if (true/* pnode->nVersion >= MIN_POOL_PEER_PROTO_VERSION */) {

                        //keep track of who we've asked for the list
                        if(!pnode->HasFulfilledRequest("mnsync")){
							pnode->FulfilledRequest("mnsync");

							LogPrintf("Successfully synced, asking for Masternode list and payment list\n");

							//request full mn list only if Masternodes.dat was updated quite a long time ago
							mnodeman.DsegUpdate(pnode);

							//pnode->PushMessage("mnget"); //sync payees
							//pnode->PushMessage("getsporks"); //get current network sporks
							g_connman->PushMessage(pnode, CNetMsgMaker(PROTOCOL_VERSION).Make(SERIALIZE_TRANSACTION_NO_WITNESS, "mnget"));
							g_connman->PushMessage(pnode, CNetMsgMaker(PROTOCOL_VERSION).Make(SERIALIZE_TRANSACTION_NO_WITNESS, "getsporks"));
							RequestedMasterNodeList++;
						}
					}
                });
            }
        }

        /*if(c % 60 == 0){
            //if we've used 1/5 of the Masternode list, then clear the list.
            if((int)vecMasternodesUsed.size() > (int)mnodeman.size() / 5)
                vecMasternodesUsed.clear();
        }

    if(darkSendPool.GetState() == POOL_STATUS_IDLE && c % 6 == 0){
     darkSendPool.DoAutomaticDenominating();
        }*/
    }
}
