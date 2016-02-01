# PebbleHealthNet

##Hack UCSC 2016
Best pebble hack!

##What it Does
HealthNet is a health analytics watchapp built using the [Pebble Health API](https://developer.getpebble.com/guides/pebble-apps/sensors/health/).
HealthNet is a tool for users to monitor their sleep, physical activity, and mood, and see correlations between the three.

HealthNet prompts users for their daily mood and then displays sleep and physical activity data integrated graphically with data about the user's mood, so that users can quickly see their recent amounts of sleep and activity and make connections with what amount of activity and sleep causes their mood to be highest.

##Can I try it out?
Currently, you can't get HealthNet on your pebble. It requires [Pebble SDK 3.9-beta7](https://developer.getpebble.com/sdk/changelogs/3.9-beta7/). 

##Inspiration
The Pebble Health app was released at the end of last year but the API is still in beta. So we decided to get in the hackathon spirit and do some real hacking.
The Pebble Health application is great at tracking your steps and sleep, but that’s about all it does at this point. We set out to make a health analytics application that gives users insights on the data Pebble has been gathering for them.
##Planned Improvements
1) We plan on adding timeline support. Our eventual goal for HealthNet is for it to offer insights throughout the day. For example, we’d love to correlate activity (step count) and sleep and send timeline notifications to the user when there are relevant insights. We'll develop an algorithm that relates a user's mood to their activity and sleep level, and we'll let the user know what conditions have them feeling best. We'll also recommend getting more sleep and being more active when our algorithm detects that a lack of one might cause them to have a worse mood. 
Examples of timeline notifications:
"You sleep 17% better on days when you've been more active than average."
"You sleep best when you to go to bed before 12am. Why not try it out tonight and see how you feel tomorrow?"

2) Expanding the graph and statistics page to display more data. We'll change the statistics page to a menu that a user can scroll through to see additional days as well as 7-, 14-, and 30-day averages with the activity, amount of sleep, and mood of the user. We'll change the graph page so that the user can also load different days and composites of time periods, rather than just the last four days.

## How we built it
Currently, you can only use the Pebble Health API with the beta Pebble SDK. HealthNet is written in C and uses only the Pebble SDK.

## Challenges I ran into
We ran into some challenges because we weren’t able to use the beta Pebble SDK with CloudPebble. This meant all of our testing had to be done on an offline emulator that was quite buggy.
Testing our application on an actual Pebble Smartwatch was another challenge. Pebble releases the beta SDK publicly but does not release the beta firmware. We had to contact Katherine Berry at Pebble in order to get an unsigned firmware update.
Oh yeah, for three out of four on our team, this was our first time using C.

##About Us
We’re three third year transfer students at UCSC and one freshmen at UCSB.
Sean Dougher gets into Dougher mode, where he stays up all night programming; we wouldn’t have an application without him. Brian put the team on his back with his expertise in C and debugging skills. Peter took charge of the tedious task of hacking the UI with Sean and writing the code that generates graphs. Michael brought the Linux so we could use the beta Pebble SDK (he also risked blowing up his Pebble Time by installing a sketchy unsigned firmware update sent via Slack). 
