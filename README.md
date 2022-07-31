# timecmd
Run commands at certain times.

Usage: timecmd [hour:minute:command]...\
  hour: 00-23, 24 hour time\
  minute: 00-59\
  command: command to be ran by system(), which uses the default shell\
  -o --once : run only once and exit, instead of looping\
  -e --exact : run only when exactly on the minute, instead of any time between it and the next one
  
Example:

- Filter blue/green from 19:00 to 06:00 on monitors eDP1 and DP1 using xrandr:\
  `timecmd "19:00:xrandr --output eDP1 --gamma 1.0:0.85:0.85 --output DP1 --gamma 1.0:0.85:0.85" "06:00:xrandr --output eDP1 --gamma 1.0:1.0:1.0 --output DP1 --gamma 1.0:1.0:1.0"`
  
- Notification to sleep at 19:00 and notification to wake up at 07:00 using notify-send:\
  `./timecmd "19:00:notify-send 'Sleep'" "07:00:notify-send 'Wake Up'"`
