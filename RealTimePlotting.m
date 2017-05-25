% Serial Data Plot
% Aditya Thakkar thakkaap

clear;
delete(instrfindall);
serialObj = serial('COM3'); % Define Serial Object
serialObj.BaudRate = 19200; %Set baud rate 
serialObj.Terminator = 'CR';
fopen(serialObj); % Open the serial object

numBits = 10; % ADC Resolution
peakVoltage = 5; % Maximum voltage 
mag = 1/(2^numBits/peakVoltage); % Division factor
dataMatrix = zeros(1,100); % Define data matrix 

while true % Loop forever
    i = 1;
while i<=100; 
    dataMatrix(1,i) = str2double(fgetl(serialObj)); % Allocate value into matrix
    i = i+1; 
 end
 
dataMatrix = mag*dataMatrix; % Voltage value
plot(dataMatrix); % Plot the data
title('Signal Graph') % Set title 
ylabel('Voltage (V)')  %Y label
xlabel('Time (s)') % X label 
axis([0, 100, 0, 5]); % Set axis 

pause(0.1); % Pause, this is also the resolution time to up date for the graph 
end