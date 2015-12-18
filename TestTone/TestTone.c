/*********************************************************************
* Author: Heming Wang
* Test Tone program:
*    TestTone.c
*
* Description:
*    This program reads signals from csv files and write analog signals to ao0 channel,
*    ao1 read signals simulteneously and write them into another csv file	
*
* Instructions for Running:
*    1. Choose the csv file to read
*    2. Enter the rate to read data
*    3. Enter the number of channels of your file
*    4. Enter the name of the output file.

*    Note: For better accuracy try to match the Input Ranges to the
*          expected voltage level of the measured signal.
* 
*    Note: The Rate you select should be at least twice as fast as the maximum
*          frequency component of the signal
*
* Steps:
*    1. Read data from csv file
*    1. Create read and write tasks for ai0 and ao0.
*    2. Create an Analog output and input Voltage channel.
*    3. Define parameters for clock source
*    4. Start writing and reading data
*    5. Repeat 4 if it's 2-channel data
*    6. Clear task
*    7. Display an error if any.
*
* I/O Connections Overview:
*	 Make sure your device name matches
*    Make sure your input/output terminal matches the physical channel I/O control
*    Connect ao0 and ai0
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <NIDAQmx.h>
#include <math.h>
#include <string.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int main()
{
	/* Initialize */
	int32  error, read = 0;
	TaskHandle writetask = 0;
	TaskHandle readtask = 0;
	char  errBuff[2048] = {'\0'};
	int mysize = 10000;  // Initial size of the data
   	int temp = 1, num = 0, i = 0; // temp->stores the result of fscanf, num -> records the number of data
	int Rate = 10000, Channel = 2; // The play rate and number of channels
    double *leftchannel;
	double *rightchannel;
	char device_name[30] = "myDAQ1";
	char inputchannel[30], outputchannel[30];
	strcpy(inputchannel, device_name); strcat(inputchannel, "/ai0");
	strcpy(outputchannel, device_name); strcat(outputchannel, "/ao0");
	/* read the csv file */
	char file_name[30];
	char output_file[30];
   	FILE *fpi;  // "input" file pointer
   	FILE *fpo;  // "output" file pointer
   	printf("Enter the name of csv file\n");
   	gets(file_name);

	/* Open file */
   	fpi = fopen(file_name,"r");
	if(fpi == NULL){
		printf("The file does not exist! Press Enter key to quit\n");
		getchar();
		return 0;
   	}
   	
	/* Set up parameters */   	
	printf("Enter the play rate of the file (Suggest: 48600)\n");
   	scanf("%d", &Rate);
	getchar();
	
	printf("Enter the number of channels for this file (1\\2)\n");
	scanf("%d", &Channel);
	getchar();
	
	if(Channel!=2 && Channel!=1){
		printf("Channel number not valid, Enter to exit\n");
		getchar();
		exit(1);
	}	
	
	leftchannel = (double*)malloc(sizeof(double)*mysize);	
	if(Channel == 2)
		rightchannel = (double*)malloc(sizeof(double)*mysize);	


   	/* Begin reading data */
	while(1){
		/* Read data, depends on channels*/
		if(Channel == 2)
			temp = fscanf(fpi,"%lf,%lf", leftchannel+num, rightchannel+num);
		else
			temp = fscanf(fpi,"%lf", leftchannel+num);
				
				
		if(temp<0){
			break;   // End reading when fscanf cannot match any data
		}
		num++;       // Data number increment
		
		/* Extend the arrays if the size is not big enough */	
		if(num >= mysize){
			mysize *= 2; // double the size			
			leftchannel = realloc(leftchannel, mysize*sizeof(double));
			if(Channel == 2)
			rightchannel = realloc(rightchannel, mysize*sizeof(double));
		}
	}
	printf("Finish reading file. The number of rows: %d.\n", num);

	/*Define arrays large enouth to receive data*/
	double** data = (double**)malloc(sizeof(double)*Channel);
	for(i = 0; i < Channel; i++){
		data[i] = (double*)malloc(sizeof(double)*num);
	}
	 
	/* DAQmx Configure Code */
	DAQmxErrChk (DAQmxCreateTask("",&writetask));
	printf("Create tasks ..... \n");
	DAQmxErrChk (DAQmxCreateAOVoltageChan(writetask,outputchannel,"",-10.0,10.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(writetask,"",Rate,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,num));
	DAQmxErrChk (DAQmxCreateTask("",&readtask));
	DAQmxErrChk (DAQmxCreateAIVoltageChan(readtask,inputchannel,"",DAQmx_Val_Cfg_Default,-10.0,10.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(readtask,"",Rate,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,num));

	
	/* DAQmx Write leftchannel */
	DAQmxErrChk (DAQmxWriteAnalogF64(writetask,num,0,10.0,DAQmx_Val_GroupByChannel,leftchannel,NULL,NULL));

	/* DAQmx Start Reading leftchannel */
	DAQmxErrChk (DAQmxStartTask(writetask));
	DAQmxErrChk (DAQmxStartTask(readtask));

	printf("Start reading leftchannel ..... \n");
	DAQmxErrChk (DAQmxReadAnalogF64(readtask,-1,10.0,DAQmx_Val_GroupByChannel,data[0],num,&read,NULL));
	if(read>0)
		printf("Acquired %d samples. ",read);
	DAQmxErrChk (DAQmxWaitUntilTaskDone(writetask,20.0));
	
	/* DAQmx Stop Task*/		
	DAQmxStopTask(writetask);
	DAQmxStopTask(readtask);
	printf("Finish reading leftchannel .... \n");
		
	if(Channel == 2){
		/* DAQmx Write rightchannel */
		DAQmxErrChk (DAQmxWriteAnalogF64(writetask,num,0,10.0,DAQmx_Val_GroupByChannel,rightchannel,NULL,NULL));
	
		/* DAQmx Start Reading rightchannel */
		DAQmxErrChk (DAQmxStartTask(writetask));
		DAQmxErrChk (DAQmxStartTask(readtask));
	
		printf("Start reading rightchannel ..... \n");
		DAQmxErrChk (DAQmxReadAnalogF64(readtask,-1,10.0,DAQmx_Val_GroupByChannel,data[1],num,&read,NULL));
		if(read>0)
			printf("Acquired %d samples. ",read);
	
		DAQmxErrChk (DAQmxWaitUntilTaskDone(writetask,20.0));

		/* DAQmx Stop Task*/		
		DAQmxStopTask(writetask);
		DAQmxStopTask(readtask);
		printf("Finish reading rightchannel .... \n");

	}
	

	/* Write data to the output file*/
	printf("Enter the file name for output data:\n");
	scanf("%s",output_file);
	getchar();	
	fpo = fopen(output_file,"w+");
	if(fpo == NULL){
		printf("Cannot write the file\n");
   	}

	if(Channel == 1){
		for(i = 0; i < num; i++)
			fprintf(fpo,"%lf\n", data[0][i]);
	}	
	else{
		for(i = 0; i < num; i++)
			fprintf(fpo,"%lf,%lf\n", data[0][i], data[1][i]);
	}
	printf("Finish writing data to %s\n", output_file);

	/* Close file pointer and free memory space */
	fclose(fpi);
	fclose(fpo);
	free(leftchannel);
	if(Channel == 2)
		free(rightchannel);
	free(data);

Error:
	if(DAQmxFailed(error)){		
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		printf("DAQmx Error: %s\n", errBuff);
	}
	if(writetask != 0){
		/* DAQmx Stop Code */
		DAQmxClearTask(writetask);
		DAQmxClearTask(readtask);
		printf("clear task .... \n");
	}
	
	printf("End of program, press Enter key to quit\n");
	getchar();
	
	return 0;

}

