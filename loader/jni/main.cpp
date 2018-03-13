#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <jni.h>
#include <stdlib.h>


int main(int argc,char** argv)
{

	JavaVM* vm;
	JNIEnv* env;
	jint res;
	
	JavaVMInitArgs vm_args;
	JavaVMOption options[1];
	options[0].optionString = "-Djava.class.path=.";
	vm_args.version=0x00010002;
	vm_args.options=options;
	vm_args.nOptions =1;
	vm_args.ignoreUnrecognized=JNI_TRUE;
	
	
	printf("[+] dlopen libdvm.so\n");
	void *handle = dlopen("/system/lib/libdvm.so", RTLD_LAZY);//RTLD_LAZY RTLD_NOW
	if(!handle){
	printf("[-] dlopen libdvm.so failed!!\n");
	return 0;
	}

	typedef int (*JNI_CreateJavaVM_Type)(JavaVM**, JNIEnv**, void*);
	JNI_CreateJavaVM_Type JNI_CreateJavaVM_Func = (JNI_CreateJavaVM_Type)dlsym(handle, "JNI_CreateJavaVM");
	if(!JNI_CreateJavaVM_Func){
	printf("[-] dlsym failed\n");
	return 0;
	}
	res=JNI_CreateJavaVM_Func(&vm,&env,&vm_args);
	dlopen("/data/local/tmp/libmctocurl.so",RTLD_LAZY);
	dlopen("/data/local/tmp/libcupid.so",RTLD_LAZY);
	void* si=dlopen("/data/local/tmp/libmcto_media_player.so",RTLD_LAZY);
	if(si == NULL)
	{
		printf("dlopen err!\n");
		return 0;
	}

	typedef char* (*FUN1)(char* plain);
    void *addr=(void*)(*(int*)((size_t)si+0x8c)+0x249BC9);
	FUN1 func=(FUN1)addr;
	if(func==NULL)
	{
		printf("can't find  func\n");
		return 0;
	}
   
	char *plain="/vps?tvid=11949478009&vid=7b23569cbed511dd58bcd6ce9ddd7b42&v=0&qypid=11949478009_unknown&src=02022001010000000000&tm=1519712402&k_tag=1&k_uid=359125052784388&bid=1&pt=0&d=1&s=0&rs=1&dfp=1413357b5efa4a4130b327995c377ebb38fbd916698ed95a28f56939e9d8825592&k_ver=9.0.0&k_ft1=859834543&k_err_retries=0&qd_v=1";
	char* ret=func(plain);
	printf("%s\n",ret);
	return 0;
}