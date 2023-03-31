device = bluetooth("Servo_Control",1);
write(device, 1:10);
ans = read(device, 10);
ans
flush(device);
%device
