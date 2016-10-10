#ifndef P645PARSE_H_
#define P645PARSE_H_


#include <stdint.h>
#include <stdbool.h>

/**********RADIO发送函数指针回调声明**********/
typedef void(*RADIO_TX_CB)(uint8_t channel,uint8_t *data,uint8_t lens);


uint8_t P645_Parse_Process(CircleQueue *CQ,PDUFRAME *PF);
uint8_t P645_Handle_Process(PDUFRAME *PF);
void App_PduType_Process(PDUFRAME *PF);
void App_Prmsec_Process(PDUFRAME *PF);
void App_Prmpri_Process(PDUFRAME *PF);
void App_Fccon_Process(PDUFRAME *PF);
void App_Fcnocon_Process(PDUFRAME *PF);
void App_Fcresdata_Process(PDUFRAME *PF);
void App_Fcnodata_Process(PDUFRAME *PF);
void App_Fcresetuse_Process(PDUFRAME *PF);
void App_Fcresetfault_Process(PDUFRAME *PF);
void App_Fcsenddatana_Process(PDUFRAME *PF);
void App_Fcsenddata_Process(PDUFRAME *PF);
void App_Fcrequestd_Process(PDUFRAME *PF);
void App_Timestamp_Process(PDUFRAME *PF);
void App_Faultwrg_Process(PDUFRAME *PF);
void App_Dbyc_Process(PDUFRAME *PF);
void Call_Wave_Save(uint8_t *au_addr);
void App_Dbyxtim_Process(PDUFRAME *PF);
void App_Dbwave_Process(PDUFRAME *PF);
void App_Dbpara_Process(PDUFRAME *PF);
void App_Dbprog_Process(PDUFRAME *PF);
void App_Led_Process(PDUFRAME *PF);
void App_Phyid_Process(PDUFRAME *PF);
void Set_P645_Frame(PDUFRAME *PF,uint8_t *ScrAddr,uint8_t *DstAddr,uint8_t ctrl,uint8_t pdutype,uint8_t framenum,uint8_t frameseq,uint8_t *data,uint8_t lens);
void Set_P645_Frame1(PDUFRAME *PF,uint8_t *ScrAddr,uint8_t *DstAddr,uint8_t ctrl,uint8_t pdutype,uint8_t framenum,uint8_t frameseq,uint8_t *data,uint8_t lens);
void Radio_Tx_Cb(RADIO_TX_CB func);
void Radio_Mid_Tx(uint8_t channel,uint8_t *data,uint8_t lens);


uint8_t Check_Crc(uint8_t *data,uint16_t lens);


#endif

