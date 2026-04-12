clc; clear; close all;

url = "http://192.168.0.123/data";   % replace with your ESP32 IP
N = 60;

temp = nan(N,1);
press = nan(N,1);
alt = nan(N,1);
ax = nan(N,1);
ay = nan(N,1);
az = nan(N,1);

figure("Color","w");
tiledlayout(3,2);

nexttile; h1 = plot(nan, nan, "LineWidth", 1.5); grid on; title("Temperature"); ylabel("C");
nexttile; h2 = plot(nan, nan, "LineWidth", 1.5); grid on; title("Pressure"); ylabel("Pa");
nexttile; h3 = plot(nan, nan, "LineWidth", 1.5); grid on; title("Altitude"); ylabel("m");
nexttile; h4 = plot(nan, nan, "LineWidth", 1.5); grid on; title("Accel X"); ylabel("m/s^2");
nexttile; h5 = plot(nan, nan, "LineWidth", 1.5); grid on; title("Accel Y"); ylabel("m/s^2");
nexttile; h6 = plot(nan, nan, "LineWidth", 1.5); grid on; title("Accel Z"); ylabel("m/s^2");

for k = 1:N
    d = webread(url);

    temp(k) = d.t;
    press(k) = d.p;
    alt(k) = d.a;
    ax(k) = d.x;
    ay(k) = d.y;
    az(k) = d.z;

    idx = 1:k;
    set(h1, "XData", idx, "YData", temp(idx));
    set(h2, "XData", idx, "YData", press(idx));
    set(h3, "XData", idx, "YData", alt(idx));
    set(h4, "XData", idx, "YData", ax(idx));
    set(h5, "XData", idx, "YData", ay(idx));
    set(h6, "XData", idx, "YData", az(idx));

    drawnow;
    pause(2);
end