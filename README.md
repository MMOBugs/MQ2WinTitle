# MQ2WinTitle

Change your EQ window titlebar (caption) text, to display data of your choice. The specific text that is displayed is configurable
via INI file. The plugin will also indicate, if you wish, if a tell was recieved in a background EQ session, by changing the
titlebar to user defined text.

## Getting Started

```
/plugin MQ2WinTitle
```

### Commands

MQ2WinTitle provides one command:

```
/wintitle
```

<span style="color: blue;">/wintitle reload</span> : <span style="color: green;">Reload configuration from ini file</span>  
<span style="color: blue;">/wintitle fixtitle [state]</span> : <span style="color: green;">Call function to 'fix' title based on game state</span>  
<span style="color: blue;">/wintitle debug [off|on]</span> : <span style="color: green;">Toggle displaying debugging messages to chat, or force on/off</span>  
<span style="color: blue;">/wintitle petdebug [off|on]</span> : <span style="color: green;">Toggle displaying pet debugging messages to chat, or force on/off</span>  
<span style="color: blue;">/wintitle [help]</span> : <span style="color: green;">This help</span>

### Configuration File

There is one section: `[Settings]`

<span style="color: blue;">InGame</span> <span style="color: green;">The titlebar format to use while in game (not zoning, foreground).</span>  
<span style="color: blue;">Tell</span> <span style="color: green;">The titlebar format to use if you recieve a tell while EQ is in the background.</span>  
<span style="color: blue;">Other</span> <span style="color: green;">The titlebar format to use at other times (such as char select).</span>  
<span style="color: blue;">Zoning</span> <span style="color: green;">The titlebar format to use while zoning.</span>  
<span style="color: blue;">ShowHotKey</span> <span style="color: green;">Whether to append the hotkey as shown by WinEQ2 or InnerSpace, if used.</span>  

The <span style="color: blue;">ShowHotKey</span> entry is either on or off. All other entries are parsed as macro parameters

## Authors

* **htw** - *Initial work*
