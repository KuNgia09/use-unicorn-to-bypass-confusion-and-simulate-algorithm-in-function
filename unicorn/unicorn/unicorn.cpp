// unicorn.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <inttypes.h>
#include <string.h>
#include <math.h>
#include "unicorn/unicorn.h"
#pragma comment(lib,"unicorn.lib")
#define _DWORD uint32_t
#define LODWORD(x)  (*((_DWORD*)&(x)))
#define HIDWORD(x)  (*((_DWORD*)&(x)+1))
#define ADDRESS 0x249BC8
#define BASE  0xaef52000
#define CODE_SIZE  8*1024*1024
#define STACK_ADDR  BASE+CODE_SIZE
#define STACK_SIZE  1024 * 1024
#define PARAM_ADDR  STACK_ADDR+STACK_SIZE
#define PARAM_SIZE  1024 * 1024
uint32_t offset=0;

static uint32_t create_mem(uc_engine *uc,char* buffer,uint32_t len)
{
	uint32_t addr = PARAM_ADDR + offset;
	uc_mem_write(uc, addr, buffer, len);
	offset += len + 1;
	return addr;
}

static void print_reg(uc_engine *uc, uint32_t address)
{
#ifdef DEBUG
	uint32_t pc = 0;
	uc_reg_read(uc, UC_ARM_REG_PC, &pc);
	if (pc == address)
	{
		printf("========================\n");        printf("Break on 0x%x\n", pc);
		uint32_t values = 0;
		uc_reg_read(uc, UC_ARM_REG_R0, &values);        printf("R0 = 0x%x \n", values);
		uc_reg_read(uc, UC_ARM_REG_R1, &values);        printf("R1 = 0x%x \n", values);
		uc_reg_read(uc, UC_ARM_REG_R2, &values);        printf("R2 = 0x%x \n", values);
		uc_reg_read(uc, UC_ARM_REG_R3, &values);        printf("R3 = 0x%x \n", values);
		uc_reg_read(uc, UC_ARM_REG_R4, &values);        printf("R4 = 0x%x \n", values);
		uc_reg_read(uc, UC_ARM_REG_R5, &values);        printf("R5 = 0x%x \n", values);
		uc_reg_read(uc, UC_ARM_REG_R6, &values);        printf("R6 = 0x%x \n", values);
		uc_reg_read(uc, UC_ARM_REG_PC, &values);        printf("PC = 0x%x \n", values);
		uc_reg_read(uc, UC_ARM_REG_SP, &values);        printf("SP = 0x%x \n", values);
		printf("========================\n");
	}
#endif 
}

static void hook_code(uc_engine *uc, uint64_t address, uint32_t size, void *user_data)
{
#ifdef DEBUG
	printf(">>> Tracing instruction at 0x%" PRIx64 ", instruction size = 0x%x\n", address, size);
#endif 
	switch (address)
	{
		case BASE + 0x249BEE:
		{
			uint32_t r0 = 0;
			char buffer[4096] = "";
			uc_reg_read(uc, UC_ARM_REG_R0, &r0);
			uc_mem_read(uc, r0, buffer, 4096);
			r0 = strlen(buffer);
			uc_reg_write(uc, UC_ARM_REG_R0, &r0);
			uint32_t pc = address;
			pc += 5;
			uc_reg_write(uc, UC_ARM_REG_PC, &pc);
			break;
		}
		case BASE+ 0x249f3c:
		case BASE+ 0x249f06:
		case BASE + 0x249c02:
		{
			uint32_t r0 = 0;
			uc_reg_read(uc, UC_ARM_REG_R0, &r0);
			char* buffer = (char*)malloc(r0);
			r0=create_mem(uc, buffer, r0);
			free(buffer);
			uc_reg_write(uc, UC_ARM_REG_R0, &r0);
			uint32_t pc = address;
			pc += 5;
			uc_reg_write(uc, UC_ARM_REG_PC, &pc);
			break;
		}
		case BASE+0x249c68:
		case BASE+0x249c0e:
		case BASE+0x24947A:
		case BASE+0x249456:
		{
			uint32_t r0 = 0;
			uint32_t r1 = 0;
			uint32_t r2 = 0;
			uc_reg_read(uc, UC_ARM_REG_R0, &r0);
			uc_reg_read(uc, UC_ARM_REG_R1, &r1);
			uc_reg_read(uc, UC_ARM_REG_R2, &r2);
			char *buffer =(char*)malloc(r2);
			uc_mem_read(uc, r1, buffer, r2);
			uc_mem_write(uc, r0, buffer, r2);
			free(buffer);
			uint32_t pc = address;
			if (address == BASE + 0x249c68)
				pc += 4;
			else
				pc += 5;
			uc_reg_write(uc, UC_ARM_REG_PC, &pc);
			break;
		}
		case BASE + 0x249C6C:
		{
			uint32_t pc = address;
			pc += 5;
			uint32_t r0 = 0x2c0;
			uc_reg_write(uc, UC_ARM_REG_R0, &r0);
			uc_reg_write(uc, UC_ARM_REG_PC, &pc);
			break;
		}
		case BASE + 0x249BD8:
		{
			uint32_t pc = address;
			pc += 7;
			uc_reg_write(uc, UC_ARM_REG_PC, &pc);
			break;
		}
		case BASE+0x249EE8:
		{
			uint32_t r0 = 0;
			uint32_t r1 = 0;
			uc_reg_read(uc, UC_ARM_REG_R0, &r0);
			uc_reg_read(uc, UC_ARM_REG_R1, &r1);
			double value = 0;
			memcpy(&value, &r0, 4);
			memcpy((char*)&value+4, &r1, 4);
			double ret=sin(value);
			r0 = LODWORD(ret);
			r1 = HIDWORD(ret);
			uc_reg_write(uc, UC_ARM_REG_R0, &r0);
			uc_reg_write(uc, UC_ARM_REG_R1, &r1);
			uint32_t pc = address;
			pc += 5;
			uc_reg_write(uc, UC_ARM_REG_PC, &pc);
			break;
		}
		case BASE+ 0x24a68c:
		case BASE+0x249f24:
		{
			uint32_t pc = address;
			pc += 5;
			uc_reg_write(uc, UC_ARM_REG_PC, &pc);
		}	
		default:
		{
			print_reg(uc, address);
			break;
		}
	}
}

static unsigned char* read_file(char* path, uint32_t* len)
{
	FILE* fp = fopen(path, "rb");
	if (fp == NULL)
		return nullptr;
	fseek(fp, 0, SEEK_END);
	*len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	unsigned char* code = (unsigned char*)malloc(*len);
	memset(code, 0, *len);
	fread(code, 1, *len, fp);
	fclose(fp);
	return code;
}

static void test_thumb(void)
{
	uc_engine *uc;
	uc_err err;
	uc_hook trace;
	uint32_t sp = STACK_ADDR; 
	offset = 0;
	err = uc_open(UC_ARCH_ARM, UC_MODE_THUMB, &uc);
	if (err) {
		printf("Failed on uc_open() with error returned: %u (%s)\n",
			err, uc_strerror(err));
		return;
	}
	char plain[] = "/vps?tvid=11949478009&vid=7b23569cbed511dd58bcd6ce9ddd7b42&v=0&qypid=11949478009_unknown&src=02022001010000000000&tm=1519712402&k_tag=1&k_uid=359125052784388&bid=1&pt=0&d=1&s=0&rs=1&dfp=1413357b5efa4a4130b327995c377ebb38fbd916698ed95a28f56939e9d8825592&k_ver=9.0.0&k_ft1=859834543&k_err_retries=0&qd_v=1";
	uc_mem_map(uc, PARAM_ADDR, PARAM_SIZE, UC_PROT_ALL);
	uc_mem_map(uc, BASE, CODE_SIZE, UC_PROT_ALL);
	uint32_t r0 = PARAM_ADDR;
	uint32_t sp_start = sp + STACK_SIZE;
	int ret=uc_mem_map(uc, sp, sp_start - sp, UC_PROT_ALL);
	uint32_t len = 0;
	unsigned char* code = read_file("./aef52000_36e000.so", &len);
	uc_mem_write(uc, BASE, code, len);
	free(code);
	create_mem(uc, plain, strlen(plain) + 1);
	uc_reg_write(uc, UC_ARM_REG_R0, &r0);
	uc_reg_write(uc, UC_ARM_REG_SP, &sp);
	uc_hook_add(uc, &trace, UC_HOOK_CODE, hook_code, NULL, 1, 0);
	err = uc_emu_start(uc, BASE + 0x249BC8 + 1, BASE + 0x24a692, 0, 0);
	if (err) {
		printf("Failed on uc_emu_start() with error returned: %u\n", err);
	}
	char buffer[4096] = "";
	uc_reg_read(uc, UC_ARM_REG_R0, &r0);
	uc_mem_read(uc, r0, buffer, 4096);
	printf("result:%s\n", buffer);
	uc_close(uc);
}

int main()
{
	test_thumb();
	system("pause");
    return 0;
}

