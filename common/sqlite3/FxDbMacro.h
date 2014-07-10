#ifndef __MACRO_EPEIUS_LIB__
#define __MACRO_EPEIUS_LIB__

#define MAX_MEMBER_NUMBER 10
#define EXPAND(...) __VA_ARGS__
#define NAME_AND_VAR(x,y) #x,x
#define PAD_COMMA ,
#define PAD_SEMICOLON ;

#define EPEIUS_DEC_1 	0
#define EPEIUS_DEC_2 	1
#define EPEIUS_DEC_3 	2
#define EPEIUS_DEC_4 	3
#define EPEIUS_DEC_5 	4
#define EPEIUS_DEC_6 	5
#define EPEIUS_DEC_7 	6
#define EPEIUS_DEC_8 	7
#define EPEIUS_DEC_9 	8
#define EPEIUS_DEC_10	9
#define EPEIUS_DEC_11	10
#define EPEIUS_DEC_12	11
#define EPEIUS_DEC_13	12
#define EPEIUS_DEC_14	13
#define EPEIUS_DEC_15	14
#define EPEIUS_DEC_16	15
#define EPEIUS_DEC_17	16
#define EPEIUS_DEC_18	17
#define EPEIUS_DEC_19	18
#define EPEIUS_DEC_20	19
#define EPEIUS_DEC_21	20
#define EPEIUS_DEC_22	21
#define EPEIUS_DEC_23	22
#define EPEIUS_DEC_24	23
#define EPEIUS_DEC_25	24
#define EPEIUS_DEC_26	25
#define EPEIUS_DEC_27	26
#define EPEIUS_DEC_28	27
#define EPEIUS_DEC_29	28
#define EPEIUS_DEC_30	29
#define EPEIUS_DEC_31	30
#define EPEIUS_DEC_32	31
#define EPEIUS_DEC_33	32
#define EPEIUS_DEC_34	33
#define EPEIUS_DEC_35	34
#define EPEIUS_DEC_36	35
#define EPEIUS_DEC_37	36
#define EPEIUS_DEC_38	37
#define EPEIUS_DEC_39	38
#define EPEIUS_DEC_40	39
#define EPEIUS_DEC_41	40
#define EPEIUS_DEC_42	41
#define EPEIUS_DEC_43	42
#define EPEIUS_DEC_44	43
#define EPEIUS_DEC_45	44
#define EPEIUS_DEC_46	45
#define EPEIUS_DEC_47	46
#define EPEIUS_DEC_48	47
#define EPEIUS_DEC_49	48
#define EPEIUS_DEC_50	49

#define TBIND_1(p,pad,...) p(__VA_ARGS__,1)
#define TBIND_2(p,pad,x,...)  p(x,2) EXPAND(PAD_##pad) EXPAND(TBIND_1(p,pad,__VA_ARGS__))
#define TBIND_3(p,pad,x,...)  p(x,3) EXPAND(PAD_##pad) EXPAND(TBIND_2(p,pad,__VA_ARGS__))
#define TBIND_4(p,pad,x,...)  p(x,4) EXPAND(PAD_##pad) EXPAND(TBIND_3(p,pad,__VA_ARGS__))
#define TBIND_5(p,pad,x,...)  p(x,5) EXPAND(PAD_##pad) EXPAND(TBIND_4(p,pad,__VA_ARGS__))
#define TBIND_6(p,pad,x,...)  p(x,6) EXPAND(PAD_##pad) EXPAND(TBIND_5(p,pad,__VA_ARGS__))
#define TBIND_7(p,pad,x,...)  p(x,7) EXPAND(PAD_##pad) EXPAND(TBIND_6(p,pad,__VA_ARGS__))
#define TBIND_8(p,pad,x,...)  p(x,8) EXPAND(PAD_##pad) EXPAND(TBIND_7(p,pad,__VA_ARGS__))
#define TBIND_9(p,pad,x,...)  p(x,9) EXPAND(PAD_##pad) EXPAND(TBIND_8(p,pad,__VA_ARGS__))
#define TBIND_10(p,pad,x,...) p(x,10) EXPAND(PAD_##pad) EXPAND(TBIND_9(p,pad,__VA_ARGS__))
#define TBIND_11(p,pad,x,...) p(x,11) EXPAND(PAD_##pad) EXPAND(TBIND_10(p,pad,__VA_ARGS__))
#define TBIND_12(p,pad,x,...) p(x,12) EXPAND(PAD_##pad) EXPAND(TBIND_11(p,pad,__VA_ARGS__))
#define TBIND_13(p,pad,x,...) p(x,13) EXPAND(PAD_##pad) EXPAND(TBIND_12(p,pad,__VA_ARGS__))
#define TBIND_14(p,pad,x,...) p(x,14) EXPAND(PAD_##pad) EXPAND(TBIND_13(p,pad,__VA_ARGS__))
#define TBIND_15(p,pad,x,...) p(x,15) EXPAND(PAD_##pad) EXPAND(TBIND_14(p,pad,__VA_ARGS__))
#define TBIND_16(p,pad,x,...) p(x,16) EXPAND(PAD_##pad) EXPAND(TBIND_15(p,pad,__VA_ARGS__))
#define TBIND_17(p,pad,x,...) p(x,17) EXPAND(PAD_##pad) EXPAND(TBIND_16(p,pad,__VA_ARGS__))
#define TBIND_18(p,pad,x,...) p(x,18) EXPAND(PAD_##pad) EXPAND(TBIND_17(p,pad,__VA_ARGS__))
#define TBIND_19(p,pad,x,...) p(x,19) EXPAND(PAD_##pad) EXPAND(TBIND_18(p,pad,__VA_ARGS__))
#define TBIND_20(p,pad,x,...) p(x,20) EXPAND(PAD_##pad) EXPAND(TBIND_19(p,pad,__VA_ARGS__))
#define TBIND_21(p,pad,x,...) p(x,21) EXPAND(PAD_##pad) EXPAND(TBIND_20(p,pad,__VA_ARGS__))
#define TBIND_22(p,pad,x,...) p(x,22) EXPAND(PAD_##pad) EXPAND(TBIND_21(p,pad,__VA_ARGS__))
#define TBIND_23(p,pad,x,...) p(x,23) EXPAND(PAD_##pad) EXPAND(TBIND_22(p,pad,__VA_ARGS__))
#define TBIND_24(p,pad,x,...) p(x,24) EXPAND(PAD_##pad) EXPAND(TBIND_23(p,pad,__VA_ARGS__))
#define TBIND_25(p,pad,x,...) p(x,25) EXPAND(PAD_##pad) EXPAND(TBIND_24(p,pad,__VA_ARGS__))
#define TBIND_26(p,pad,x,...) p(x,26) EXPAND(PAD_##pad) EXPAND(TBIND_25(p,pad,__VA_ARGS__))
#define TBIND_27(p,pad,x,...) p(x,27) EXPAND(PAD_##pad) EXPAND(TBIND_26(p,pad,__VA_ARGS__))
#define TBIND_28(p,pad,x,...) p(x,28) EXPAND(PAD_##pad) EXPAND(TBIND_27(p,pad,__VA_ARGS__))
#define TBIND_29(p,pad,x,...) p(x,29) EXPAND(PAD_##pad) EXPAND(TBIND_28(p,pad,__VA_ARGS__))
#define TBIND_30(p,pad,x,...) p(x,30) EXPAND(PAD_##pad) EXPAND(TBIND_29(p,pad,__VA_ARGS__))
#define TBIND_31(p,pad,x,...) p(x,31) EXPAND(PAD_##pad) EXPAND(TBIND_30(p,pad,__VA_ARGS__))
#define TBIND_32(p,pad,x,...) p(x,32) EXPAND(PAD_##pad) EXPAND(TBIND_31(p,pad,__VA_ARGS__))
#define TBIND_33(p,pad,x,...) p(x,33) EXPAND(PAD_##pad) EXPAND(TBIND_32(p,pad,__VA_ARGS__))
#define TBIND_34(p,pad,x,...) p(x,34) EXPAND(PAD_##pad) EXPAND(TBIND_33(p,pad,__VA_ARGS__))
#define TBIND_35(p,pad,x,...) p(x,35) EXPAND(PAD_##pad) EXPAND(TBIND_34(p,pad,__VA_ARGS__))
#define TBIND_36(p,pad,x,...) p(x,36) EXPAND(PAD_##pad) EXPAND(TBIND_35(p,pad,__VA_ARGS__))
#define TBIND_37(p,pad,x,...) p(x,37) EXPAND(PAD_##pad) EXPAND(TBIND_36(p,pad,__VA_ARGS__))
#define TBIND_38(p,pad,x,...) p(x,38) EXPAND(PAD_##pad) EXPAND(TBIND_37(p,pad,__VA_ARGS__))
#define TBIND_39(p,pad,x,...) p(x,39) EXPAND(PAD_##pad) EXPAND(TBIND_38(p,pad,__VA_ARGS__))
#define TBIND_40(p,pad,x,...) p(x,40) EXPAND(PAD_##pad) EXPAND(TBIND_39(p,pad,__VA_ARGS__))
#define TBIND_41(p,pad,x,...) p(x,41) EXPAND(PAD_##pad) EXPAND(TBIND_40(p,pad,__VA_ARGS__))
#define TBIND_42(p,pad,x,...) p(x,42) EXPAND(PAD_##pad) EXPAND(TBIND_41(p,pad,__VA_ARGS__))
#define TBIND_43(p,pad,x,...) p(x,43) EXPAND(PAD_##pad) EXPAND(TBIND_42(p,pad,__VA_ARGS__))
#define TBIND_44(p,pad,x,...) p(x,44) EXPAND(PAD_##pad) EXPAND(TBIND_43(p,pad,__VA_ARGS__))
#define TBIND_45(p,pad,x,...) p(x,45) EXPAND(PAD_##pad) EXPAND(TBIND_44(p,pad,__VA_ARGS__))
#define TBIND_46(p,pad,x,...) p(x,46) EXPAND(PAD_##pad) EXPAND(TBIND_45(p,pad,__VA_ARGS__))
#define TBIND_47(p,pad,x,...) p(x,47) EXPAND(PAD_##pad) EXPAND(TBIND_46(p,pad,__VA_ARGS__))
#define TBIND_48(p,pad,x,...) p(x,48) EXPAND(PAD_##pad) EXPAND(TBIND_47(p,pad,__VA_ARGS__))
#define TBIND_49(p,pad,x,...) p(x,49) EXPAND(PAD_##pad) EXPAND(TBIND_48(p,pad,__VA_ARGS__))
#define TBIND_50(p,pad,x,...) p(x,50) EXPAND(PAD_##pad) EXPAND(TBIND_49(p,pad,__VA_ARGS__))

#define TBIND(n,pad,opt,...) EXPAND(TBIND_##n(opt,pad,__VA_ARGS__))

#define RBIND_1(p,pad,x) p(x,1)
#define RBIND_2(p,pad,x,...)  EXPAND(RBIND_1(p,pad,__VA_ARGS__))  EXPAND(PAD_##pad) p(x,2)
#define RBIND_3(p,pad,x,...)  EXPAND(RBIND_2(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,3)
#define RBIND_4(p,pad,x,...)  EXPAND(RBIND_3(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,4)
#define RBIND_5(p,pad,x,...)  EXPAND(RBIND_4(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,5)
#define RBIND_6(p,pad,x,...)  EXPAND(RBIND_5(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,6)
#define RBIND_7(p,pad,x,...)  EXPAND(RBIND_6(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,7)
#define RBIND_8(p,pad,x,...)  EXPAND(RBIND_7(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,8)
#define RBIND_9(p,pad,x,...)  EXPAND(RBIND_8(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,9)
#define RBIND_10(p,pad,x,...) EXPAND(RBIND_9(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,10)
#define RBIND_11(p,pad,x,...) EXPAND(RBIND_10(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,11)
#define RBIND_12(p,pad,x,...) EXPAND(RBIND_11(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,12)
#define RBIND_13(p,pad,x,...) EXPAND(RBIND_12(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,13)
#define RBIND_14(p,pad,x,...) EXPAND(RBIND_13(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,14)
#define RBIND_15(p,pad,x,...) EXPAND(RBIND_14(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,15)
#define RBIND_16(p,pad,x,...) EXPAND(RBIND_15(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,16)
#define RBIND_17(p,pad,x,...) EXPAND(RBIND_16(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,17)
#define RBIND_18(p,pad,x,...) EXPAND(RBIND_17(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,18)
#define RBIND_19(p,pad,x,...) EXPAND(RBIND_18(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,19)
#define RBIND_20(p,pad,x,...) EXPAND(RBIND_19(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,20)
#define RBIND_21(p,pad,x,...) EXPAND(RBIND_20(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,21)
#define RBIND_22(p,pad,x,...) EXPAND(RBIND_21(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,22)
#define RBIND_23(p,pad,x,...) EXPAND(RBIND_22(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,23)
#define RBIND_24(p,pad,x,...) EXPAND(RBIND_23(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,24)
#define RBIND_25(p,pad,x,...) EXPAND(RBIND_24(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,25)
#define RBIND_26(p,pad,x,...) EXPAND(RBIND_25(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,26)
#define RBIND_27(p,pad,x,...) EXPAND(RBIND_26(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,27)
#define RBIND_28(p,pad,x,...) EXPAND(RBIND_27(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,28)
#define RBIND_29(p,pad,x,...) EXPAND(RBIND_28(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,29)
#define RBIND_30(p,pad,x,...) EXPAND(RBIND_29(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,30)
#define RBIND_31(p,pad,x,...) EXPAND(RBIND_30(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,31)
#define RBIND_32(p,pad,x,...) EXPAND(RBIND_31(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,32)
#define RBIND_33(p,pad,x,...) EXPAND(RBIND_32(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,33)
#define RBIND_34(p,pad,x,...) EXPAND(RBIND_33(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,34)
#define RBIND_35(p,pad,x,...) EXPAND(RBIND_34(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,35)
#define RBIND_36(p,pad,x,...) EXPAND(RBIND_35(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,36)
#define RBIND_37(p,pad,x,...) EXPAND(RBIND_36(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,37)
#define RBIND_38(p,pad,x,...) EXPAND(RBIND_37(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,38)
#define RBIND_39(p,pad,x,...) EXPAND(RBIND_38(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,39)
#define RBIND_40(p,pad,x,...) EXPAND(RBIND_39(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,40)
#define RBIND_41(p,pad,x,...) EXPAND(RBIND_40(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,41)
#define RBIND_42(p,pad,x,...) EXPAND(RBIND_41(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,42)
#define RBIND_43(p,pad,x,...) EXPAND(RBIND_42(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,43)
#define RBIND_44(p,pad,x,...) EXPAND(RBIND_43(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,44)
#define RBIND_45(p,pad,x,...) EXPAND(RBIND_44(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,45)
#define RBIND_46(p,pad,x,...) EXPAND(RBIND_45(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,46)
#define RBIND_47(p,pad,x,...) EXPAND(RBIND_46(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,47)
#define RBIND_48(p,pad,x,...) EXPAND(RBIND_47(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,48)
#define RBIND_49(p,pad,x,...) EXPAND(RBIND_48(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,49)
#define RBIND_50(p,pad,x,...) EXPAND(RBIND_49(p,pad,__VA_ARGS__)) EXPAND(PAD_##pad) p(x,50)

#define RBIND(n,pad,opt,...) EXPAND(RBIND_##n(opt,pad,__VA_ARGS__))

#define ONE_NUM(x,y) y
#define NUM_SEQ_MACRO(n,...) RBIND(n,COMMA,ONE_NUM,__VA_ARGS__)

#define TYPE_PARAM(x,y) typename T##y
#define FUN_PARAM_MAKETUPLE(x,y) T##y const &p##y
#define FUN_IMPL_MAKETUPLE(x,y) p##y
#define TYPE_IMPL_PARAM(x,y) T##y

#define CONSTRUCT(n,p) TBIND(n,SEMICOLON,p,NUM_SEQ_MACRO(n))

#endif
