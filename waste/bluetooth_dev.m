function outchar = bluetooth_dev()
      clc;
    clear;
    scom = serial('COM6');
    scom.InputBufferSize =400;
    scom.OutputBufferSize =400;
    scom.ReadAsyncMode='continuous';
    scom.BaudRate =9600;
    scom.Parity ='none';
    scom.StopBits =1;
    scom.DataBits =8;
    scom.Terminator ='CR';
    scom.FlowControl ='none';
    scom.timeout =1;
    scom.BytesAvailableFcnMode = 'byte';
    scom.BytesAvailableFcnCount = 1024;
    scom.BytesAvailableFcn = @callback;

    %set(scom,'Timeout',15);
    fclose(instrfind);
    fopen(scom);

    fid = fopen('angles.txt','r');
    filename = '.\angles.txt';
    send_data = fgetl(fid);
    %send_data = textread(filename, '%s',1);

    %send_data = '45 66 78 255';
    fwrite(scom,send_data, 'uint8', 'async') %数据写入串口

    %pause(2);
    %if scom.BytesAvailable == 0
     %   return 
    %end

    outchar=fscanf(scom);%从串口读数据
    outchar
    %succeed = 1;
    %succeed;

    %scom = handles.sbuff      % 传递对象句柄
    % 尝试停止、关闭删除串口对象
    stopasync(instrfind)      % 停止串口异步读写
    fclose(instrfind);        % 关闭串口设备对象
    delete(instrfind);        % 删除内存中的串口设备对象
    clear scom;               % 清除工作区间的串口设备对象
    %fclose(scom);
    fclose(fid);
end