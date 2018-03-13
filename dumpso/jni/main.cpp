#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <jni.h>
#include <stdlib.h>


int main(int argc,char** argv)
{
	
	if(argc<2)
	{
		printf("usage:./dump ./libxx.so /system/lib/libc.so ...");
		return 0;
	}
	else if (argc>3)
	{
	}
	{
		for(int i=2;i<argc;i++)
		{
			printf("[+] dlopen %s\n",argv[i]);
			void* si=dlopen(argv[i],RTLD_LAZY);
			if(si == NULL)
			{
				printf("[-] dlopen depend so err!\n");
				return 0;
			}
		}
	}
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
	void* si=dlopen(argv[1],RTLD_LAZY);
	if(si == NULL)
	{
		printf("[-] dlopen err!\n");
		return 0;
	}

    void *addr=(void*)(*(int*)((size_t)si+0x8c));
	uint32_t len=*(uint32_t*)((size_t)si+0x90);
	char path[257]="";
	sprintf(path,"%p_0x%x.so",addr,len);
	FILE* fp=fopen(path,"wb");
	printf("[+] save %s\n",path);
	fwrite(addr,1,len,fp);
	fclose(fp);
	return 0;
}