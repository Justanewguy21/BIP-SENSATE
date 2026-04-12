%% Getting data from url
url = "http://192.168.0.247/data";
opts = weboptions("Timeout", 50, "ContentType", "text");
raw = webread(url)
%% extracting data
temp = zeros(1,20);
pressure  = zeros(1,20); 
for x = 1:20
    d = webread(url);
    temp(x) = data.t;
    pressure(x) = data.p;
    pause(2);

end


%% create vectors
X = 1:20;
%temp
subplot(2,1,1);
plot(X,temp)
title('Temperature')
xlabel('Time')
ylabel('Temp(C)')
%pressure
subplot(2,1,2); 
plot(X, pressure, 'y')
title('Pressure')
xlabel('Time')
ylabel('Pressure(Pa)')
