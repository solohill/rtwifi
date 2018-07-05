//ä¿®æ”¹ 2018.2.19
#include <net/tdma.h>
#include "ath9k.h"
#include <asm/div64.h>
/*
mod = do_div(x,y);
result = x; 
*/

void tdma_tasklet(unsigned long data)
{
    struct ath_softc *sc = (struct ath_softc *)data;
    struct ieee80211_hw *hw=sc->hw;
    struct ath_hw *ah=sc->sc_ah;
    u64 tsf = ath9k_hw_gettsf64(ah);
    int slot=tsf_to_slot(tsf);
    if(ah->opmode==NL80211_IFTYPE_AP){
        static bool send_beacon=false;
        bool beacon_slot=is_AP_beacon_slot(slot);
        bool data_slot=is_data_slot(slot, NL80211_IFTYPE_AP);
        if(beacon_slot&&send_beacon==false){
            send_beacon=true;
            ath9k_beacon_tasklet(data);
            return;
            //printk("Slot = %llu, beacon_tasklet is acivate at %llu\n",slot,tsf);
        }
        if(data_slot){
            send_beacon=false;
            tdma_send_data(hw);
        }
    }else if(ah->opmode==NL80211_IFTYPE_STATION){
        bool data_slot=is_data_slot(slot, NL80211_IFTYPE_STATION);
        if(data_slot){
            tdma_send_data(hw);
        } 
    }
    
}

/*

        if(slot>3){
            send_beacon=0;
            int tempslot=slot;
            if(tempslot%2==0){
                set_tdma_slot(0);
                return;
            }
            //printk("Slot = %llu, send data is acivate at %llu\n",slot,tsf);
            set_tdma_slot(1);
            tdma_send_data(hw);
        }

         if(tempslot>3&&tempslot%2==0){
            set_tdma_slot(1);
            tdma_send_data(hw);
            //printk("Slot = %llu, beacon_tasklet is acivate at %llu\n",slot,tsf);
        }else{
            set_tdma_slot(0);
        }   
        u32 val=REG_READ(ah, AR_DIAG_SW);
                    if(val&AR_DIAG_FORCE_CH_IDLE_HIGH){
                        printk("--------AR_DIAG_FORCE_CH_IDLE_HIGH---------\n");
                    }
*/