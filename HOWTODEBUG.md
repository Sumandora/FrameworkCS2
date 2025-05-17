# How to debug crashes
## Step 1: Security
Using a debugger is technically not forbidden by Valve, since you are supposed to use it to report issues on their behalf as well.  
It is still recommended to isolate your debug environment from your actual environment.  
To what extent you want to isolate these environments is being left open for you to decide.

## Step 2: Starting steam
You want to start steam using a terminal, so you can read the output log.  
If you just closed steam it may take a while until your terminal is allowed to start a new session, because the lock isn't removed yet.  
You can now start the game.

## Step 3: Building and attaching the debugger
You should see a `Debug.sh` file in the project root.  
Run it using another terminal, as root user.  

## Step 4: Using the debugger
After running the script you probably see a message like the following in your terminal:
```
Using host libthread_db library "/lib64/libthread_db.so.1".
0x00007f0be0af579a in __GI___clock_nanosleep (clock_id=clock_id@entry=0, flags=flags@entry=0, req=0x7ffc392de680, rem=0x7ffc392de670) at ../sysdeps/unix/sysv/linux/clock_nanosleep.c:48
```
The execution of the game should automatically continue.  
If not jump to Appendix A.

# Step 5: Crashing the game
This is the simplest step of them all, just do whatever combination of events you figured out make the program crash.  
As soon as the game crashed you should look at the terminal.  
You can now jump to Appendix A.  

You are not under time pressure.  
It may take hours for the game to crash.  
As long as it crashes with the debugger attached, you're good.  
You may play normally if you don't exactly know what is causing the crash.

## Appendix A: Inspecting an automatic break
Enter 'bt'  
You should see a call stack, like this one:
```
(gdb) bt
#0  0x00007f0be0af579a in __GI___clock_nanosleep (clock_id=clock_id@entry=0, flags=flags@entry=0, 
    req=0x7ffc392de680, rem=0x7ffc392de670) at ../sysdeps/unix/sysv/linux/clock_nanosleep.c:48
#1  0x00007f0be0afa003 in __GI___nanosleep (req=<optimized out>, rem=<optimized out>)
    at ../sysdeps/unix/sysv/linux/nanosleep.c:25
#2  0x00007f0bddb03ee5 in ?? ()
   from ~/.local/share/Steam/steamapps/common/Counter-Strike Global Offensive/bin/linux64/libSDL2-2.0.so.0
#3  0x00007f0bdda3acb9 in ?? ()
   from ~/.local/share/Steam/steamapps/common/Counter-Strike Global Offensive/bin/linux64/libSDL2-2.0.so.0
#4  0x00007f0bd4f19675 in ?? ()
   from ~/.local/share/Steam/steamapps/common/Counter-Strike Global Offensive/bin/linux64/engine_client.so
#5  0x00007f0bd4f13bb2 in ?? ()
   from ~/.local/share/Steam/steamapps/common/Counter-Strike Global Offensive/bin/linux64/engine_client.so
```
The experienced reader might be able to tell that this isn't a crash because I broke the execution of program at a random point in time.  
Your output probably looks different.  
If you are not able to decide whose fault it was then write the call stack down, preferably using a digital writer.  
If your terminal got a scroll-back buffer, you can also let the terminal temporarily keep it.  
Now enter `c`.  
If the game continues execution like expected then go back to the last step and follow as if nothing happened.  
If the game closes now you are done, go to Appendix B.

## Appendix B: Confetti
At this point you can take the steam log and the gdb log and supply them in an issue or related.
