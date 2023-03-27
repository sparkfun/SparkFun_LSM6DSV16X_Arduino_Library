#include "sfe_lsm_shim.h"
#include "sfe_lsm6dsv16x.h"


// Initializes the interfacing struct for STMicroelectronic's library. 
void initCtx(void* handle, stmdev_ctx_t* dev){

	dev->handle = handle; 	
	dev->write_reg = sfeLSMWrite;
	dev->read_reg = sfeLSMRead;
}

// Points to the write method in the LSM6DSV16X Arduino Library
int32_t sfeLSMWrite(void* fTarget, uint8_t reg, const uint8_t *bufp, uint16_t len)
{
	return (((QwDevLSM6DSV16X*)fTarget)->writeRegisterRegion(reg, (uint8_t*)bufp, len));
}

// Points to the read method in the LSM6DSV16X Arduino Library
int32_t sfeLSMRead(void* fTarget, uint8_t reg, uint8_t *bufp, uint16_t len)
{
    return (((QwDevLSM6DSV16X*)fTarget)->readRegisterRegion(reg, bufp, len));
}



