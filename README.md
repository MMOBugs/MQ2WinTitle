# MQ2WinTitle

Change your EQ window titlebar (caption) text, to display data of your choice. The specific text that is displayed is configurable
via INI file. The plugin will also indicate, if you wish, if a tell was recieved in a background EQ session, by changing the
titlebar to user defined text.

## Getting Started

```txt
/plugin MQ2WinTitle
```

### Commands

MQ2WinTitle provides one command and several options for configuration:

```txt
/winload
```

Forces immediate reload of the MQ2WinTitle.ini entries.

### Configuration File

There is one section, titled: `[Settings]`

InGame The titlebar format to use while in game (not zoning, foreground).  
Tell The titlebar format to use if you recieve a tell while EQ is in the background.  
Other The titlebar format to use at other times (such as char select).  
Zoning The titlebar format to use while zoning.  
ShowHotKey Whether to append the hotkey as shown by WinEQ2 or InnerSpace, if used.  
KeepOriginalTitle Whether to append the MQ2WinTitle entry to the original title or not. If this is off, then any entry will overwrite the original title.  

The ShowHotKey entry is either on or off. All other entries are parsed as macro parameters

## Authors

* **htw** - *Initial work*
