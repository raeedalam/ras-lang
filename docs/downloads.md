# Downloading RAS Lang
Downloading RAS Lang is very easy, well at some parts,

First, Go onto the [RAS Lang Github page](github.com/raeedalam/ras-lang) and, Find *Releases* which is on the right side

After that Download the Linux or Windows download

# Linux
After downloading, Go into your terminal and enter the folder where you download the file.
To install it `ras-lang` type:
```
mv ras-lang-linux /usr/local/bin/ras-lang
chmod +x /usr/local/bin/ras-lang
```

Congrats! You installed `ras-lang` in Linux! You can type `ras-lang create <name>` to create a project then type `ras-lang <name>` to run the app.

# Windows 10
Download the file, then find `This PC` and right click, after click `Properties`

![Image](https://helpdeskgeek.com/wp-content/pictures/2017/09/computer-properties.png)

On the System dialog page, you’ll see an Advanced system settings link on the left-hand side.

![Image](https://helpdeskgeek.com/wp-content/pictures/2017/09/advanced-system-settings.png)

This will bring up the System Properties dialog, which should already be open to the Advanced tab. Go ahead and click on the Environment Variables button at the very bottom.

![Image](https://helpdeskgeek.com/wp-content/pictures/2017/09/environment-variables.png)

On the Environment Variables dialog, you’ll see two sets of variables: one for user variables and the other for system variables. Both lists have the PATH variable, so you have to decide which one to edit.

![Image](https://helpdeskgeek.com/wp-content/pictures/2017/09/edit-environment-variables.png)

![Image](https://helpdeskgeek.com/wp-content/pictures/2017/09/add-new-envrionment-variable.png)

Press `New` then paste the location of your downloaded file there. 

Once this is done, you should be able to type `ras-lang` in CMD!
