#1号口为读入口(串口设备读入数据的口子),2号口为写出口(串口向外面写数据)
#! /usr/bin/env python  
 
#coding=utf-8  
 
import pty  
import os  
import select  
 
def mkpty():  
    #  
    master1, slave = pty.openpty()  
    slaveName1 = os.ttyname(slave)  
    master2, slave = pty.openpty()  
    slaveName2 = os.ttyname(slave)  
    print ( '\nslave device names: ', slaveName1, slaveName2 )
    return master1, master2  
 
 
if __name__ == "__main__":  
 
    master1, master2 = mkpty()  
    while True:          
        rl, wl, el = select.select([master1,master2], [], [], 1)  
        for master in rl:  
            data = os.read(master, 128)  
            if master==master1:  
                print ( "read %d data:" % len(data) )
                print (data) 
                #os.write(master2, data)  
            else:  
                print ( "to write %d data:" % len(data) )  
                print ( data.strip() )
                os.write(master1, data.strip()) 