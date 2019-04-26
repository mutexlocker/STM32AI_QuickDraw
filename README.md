# QuickDraw AI Game On STM32
In this project we have used STM32F429I-DISC board to create an AI game inspired by Google's Quick, Draw! AI experiment  (https://quickdraw.withgoogle.com/).

![alt text](https://i.imgur.com/H1eiF5U.png "Hand written digits recognition on STM32F4")

## Demo 
Link to demo video : https://www.youtube.com/watch?v=8oBEGNwsj4A



## Traning 
Tensoflow 2.0 is used to train and save network in JSON and h5 format. More details and python code for training  is here:
[https://github.com/nimaaghli/quickdrawTF2]
## What it does
Upon starting, the console shows a random keyword out of 100 categories and the user has 5 seconds to think and  20 seconds to draw the keyword on the screen. After 20 seconds system converts the user's drawing to an image and feeds it to the neural network. If the drawing is recognized as one of the top five output probabilities, then the system considers it as a correct drawing for the keyword. 
STM32F429 ARM microcontroller is used as the main processor. It has 2 Mbytes of Flash memory and 256 Kbytes of RAM. Our neural network only uses 1.42MB of flash and needs 103KBytes of RAM for inference.  

## How I built it
We trained Convolutional Neural Network with 100 categories from Google`s doodle drawings dataset using TensoFlow 2.0. Our network accuracy for TOP5 category was 95%.

## Neural Network On STM32 
STM32CUBEMX.AI is used to integrate the trained neural network to STM32F429I-DISC. STM32CUBEMX configuration file is *STM32AI_QuickDraw.ioc*
## Drivers 
STM32F429I-DISC BSP drivers are used to drive LCD and touch screen. 
## IDE
The firmware is compiled by  SW4STM32 IDE. This repository contains configuration for this IDE.
