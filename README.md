# EasyDSP prototype development repository.
This is the EasyDSP prototype development library. In this repository you will find the hardware of the EasyDSP as well as the corresponding firmware. In this readme you will find an explanation of how to firmware functions and how to control the EasyDSP using the corresponding communication protocol. Also, you will find the PCB schematics and PCB design without the need to have KiCad and all the corresponding components on hand.

## Table of contents:
- [EasyDSP prototype development repository.](#easydsp-prototype-development-repository)
  - [Table of contents:](#table-of-contents)
  - [Introduction ](#introduction-)
  - [Communication and protocol ](#communication-and-protocol-)
  - [Firmware ](#firmware-)
  - [Hardware ](#hardware-)

## Introduction <a name="Introduction"></a>
The goal of this project is to make a cheap and easy to control module in order to do all your audio digital signal processing needs. The EasyDSP could be used for audio crossover, room correction, subwoofer tuning, using [Bass Eq](https://www.avsforum.com/threads/bass-eq-for-filtered-movies.2995212/) and much more. The goal of the project is to be able to control the EasyDSP using a USB or Bluetooth low energy connection. When connected easy commands can be used to control the EasyDSP. The basic structure of the signal processor can be explained with the following image:
</br>![Basic DSP overview](functional_design_system.png)</br>
The EasyDSP has two inputs and four outputs. The mux makes it possible to connect any input with any output. Every in and output has an equalizer to which five filters can be applied.</br>
The goal of the protoype is to implement the basic functioning of this system while doing it reliably. When this is achieved, functionality can be upgraded.

## Communication and protocol <a name="Communication"></a>
Explain communication to the EasyDSP.
## Firmware <a name="Firmware"></a>
Explain how to firmware is written.
## Hardware <a name="Hardware"></a>
Tell something about the hardware.