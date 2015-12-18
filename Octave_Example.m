>> clear
>> y = wavread("TestTone/sample.wav");

>> plot(y(:,1));
>> system("TestTone.exe")

Enter the name of csv file
test1.csv
Enter the play rate of the file (Suggest: 48600)
46000
Enter the number of channels for this file (1\2)
2
Finish reading file. The number of rows: 82652.
Create tasks .....
Start reading leftchannel .....
Acquired 82652 samples. Finish reading leftchannel ....
Start reading rightchannel .....
Acquired 82652 samples. Finish reading rightchannel ....
Enter the file name for output data:
testoutput.csv
Finish writing data to testoutput.csv
clear task ....
End of program, press Enter key to quit

ans = 0
>> yprime = csvread("testoutput.csv");
>> plot(yprime(:,1));