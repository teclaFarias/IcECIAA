#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Jun  8 17:12:30 2018

@author: tecla
"""

import sys
import os
#import string
import datetime
import time
import csv, operator
from PyQt5 import QtWidgets
from PyQt5 import QtGui
from PyQt5 import QtCore
from PyQt5.QtGui import *
from PyQt5.QtCore import Qt
from PyQt5.QtCore import QThread, QTimer, QEventLoop
from PyQt5.QtWidgets import QApplication, QMainWindow, QDockWidget, QDialog

import pyqtgraph as pg
#import pyqtgraph.graphicsWindows
from datetime import datetime, date, time, timedelta

#from pyqtgraph import PlotWidget as pg

import pandas as pd
#from pandas import Series, DataFrame, Panel
import numpy as np
import threading
import multiprocessing
from threading import *

from PyQt5.QtWidgets import  QMenu, QVBoxLayout, QSizePolicy, QMessageBox, QWidget, QPushButton, QLineEdit
from PyQt5.QtGui import QIcon

import interfaz
#import nvoProyecto

import serial
import re
import matplotlib.pyplot as plt
import matplotlib.animation as animation

from interfaz import Ui_Dialog
from nvoProyecto import Ui_nvoProyecto
from alerta import Ui_alerta
from ayuda import Ui_ayuda


from lxml import etree


from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

from pathlib import Path


#Aplicativos
from aplicativos.perceptron.perceptron import perceptron
from aplicativos.memoriaSD.vent_memoriaSD import Ui_memoriaSD  # borrar
from aplicativos.memoriaSD.memoriaSD import memoriaSD

"""
***********************************************
"""

class SerialCom:
    import codecs
    def __init__(self,puerto,baudios):
        self.ser = serial.Serial()
        self.ser.baudrate = baudios
        self.ser.port = puerto
        self.ser.open()

    def leePuerto(self):
        try:
            dato= self.ser.readline()
            return dato
        except:
            pass
    
    def escribePuerto(self,dato):
        try:
            self.ser.write(dato.encode('ASCII') )
        except:
            pass
        
    def escribeDatoPuerto(self,dato):
        try:
            self.ser.write(dato)
        except:
            pass
    
    def cierraPuerto(self):
        self.ser.close()

    def limpiaPuerto(self):
        self.ser.flush()
        self.ser.flushInput()
        self.ser.flushOutput()

class Cola:

    def __init__(self, elementos=20):
        """ Crea una cola vacía. """
        self.items= []
        
    def encolar(self, x):
        """ Agrega el elemento x como último de la cola. """
        self.items.append(x)

    def desencolar(self):
        """ Elimina el primer elemento de la cola y devuelve su
            valor. Si la cola está vacía, levanta ValueError. """
        try:
            return self.items.pop(0)
        except:
            raise ValueError("La cola está vacía")
            
    def es_vacia(self):
        """ Devuelve True si la cola esta vacía, False si no lo está."""
        return self.items == []
    
    def logitud(self):
        """ Devuelve la longitud de la cola """
        return len(self.items)


class AppWindow(QDialog, interfaz.Ui_Dialog ):

    def __init__(self):
        super().__init__()
        QtWidgets.QMainWindow.__init__(self)
        QThread.__init__(self)
        self.ui = Ui_Dialog()
        self.ui.setupUi(self) 
        self.ui.label_4.setPixmap(QtGui.QPixmap("IcECIAA/imagenes/unsamlogo.png"))
        self.ui.pushButton.setIcon(QtGui.QIcon("IcECIAA/imagenes/compilaM0.png"))
        self.ui.pushButton_2.setIcon(QtGui.QIcon("IcECIAA/imagenes/compilaM4.png"))
        self.ui.pushButton_4.setIcon(QtGui.QIcon("IcECIAA/imagenes/grabarM0.png"))
        self.ui.pushButton_3.setIcon(QtGui.QIcon("IcECIAA/imagenes/grabarM4.png"))
        self.ui.pushButton_11.setIcon(QtGui.QIcon("IcECIAA/imagenes/salvavidas.png"))
        self.pathRx='/tmp/'
        self.buffer=100
        self.contador=0
        self.inicio= 0
        self.delayBotonesTx= 0.3
        self.adquirirDatos= False
        self.datosPerdidos=0
        self.habilitaTerminalRx= True
        
        self.MsjSuspenderRX = 1
        self.q= Cola(self.buffer)
        
        self.thPloteo = threading.Thread(target=self.loop)
        self.thTerminalRx = threading.Thread(target=self.terminalRx)
#        self.thPloteo.daemon=True


        self.iniciaBotonesTx()
        self.iniciaBotonesRx()
        self.listaProyectos()   
        self.listaPuertosSerie()
        self.listaPuertosTx()
        self.listaBaudiosTx()
        self.listaPuertosRx()
        self.listaCanalesRx()
        self.listaBaudiosRx()
        self.listaScripts()
        self.listaAplicativos()
        self.Conexion()
        
        self.show()  


    def finalizarApp(self):
        self.cancelarRx()
        self.cancelarTx()            
        print('Finalizó correctamente')
        import sys
        sys.exit(0)


    def iniciaBotonesTx(self):
        self.ui.ButtonTxA.setStyleSheet("background-color: red;")
        self.ui.ButtonTxA.setEnabled(False)
        self.ui.ButtonTxB.setStyleSheet("background-color: red;")
        self.ui.ButtonTxB.setEnabled(False)    
        self.ui.ButtonTxC.setStyleSheet("background-color: red;")
        self.ui.ButtonTxC.setEnabled(False)
        self.ui.ButtonTxD.setStyleSheet("background-color: red;")
        self.ui.ButtonTxD.setEnabled(False)    
        self.ui.ButtonTxE.setStyleSheet("background-color: red;")
        self.ui.ButtonTxE.setEnabled(False)
        self.ui.ButtonTxF.setStyleSheet("background-color: red;")
        self.ui.ButtonTxF.setEnabled(False)    
        self.ui.ButtonTxG.setStyleSheet("background-color: red;")
        self.ui.ButtonTxG.setEnabled(False)    
        self.ui.ButtonTxH.setStyleSheet("background-color: red;")
        self.ui.ButtonTxH.setEnabled(False)    


        self.ui.ButtonTxA.clicked.connect(self.datoA_Tx)
        self.ui.lineEdit.textChanged.connect(self.pintaBotonA)
        self.ui.ButtonTxB.clicked.connect(self.datoB_Tx)
        self.ui.lineEdit_2.textChanged.connect(self.pintaBotonB)
        self.ui.ButtonTxC.clicked.connect(self.datoC_Tx)
        self.ui.lineEdit_3.textChanged.connect(self.pintaBotonC)
        self.ui.ButtonTxD.clicked.connect(self.datoD_Tx)
        self.ui.lineEdit_4.textChanged.connect(self.pintaBotonD)
        self.ui.ButtonTxE.clicked.connect(self.datoE_Tx)
        self.ui.lineEdit_5.textChanged.connect(self.pintaBotonE)
        self.ui.ButtonTxF.clicked.connect(self.datoF_Tx)
        self.ui.lineEdit_6.textChanged.connect(self.pintaBotonF)    
        self.ui.ButtonTxG.clicked.connect(self.datoG_Tx)
        self.ui.lineEdit_7.textChanged.connect(self.pintaBotonG)    
        self.ui.ButtonTxH.clicked.connect(self.datoH_Tx)
        self.ui.spinBox.valueChanged.connect(self.pintaBotonH)
        self.ui.ButtonTxScp.clicked.connect(self.datoScp_Tx)
        self.ui.pushButton_9.clicked.connect(self.guardarScript)
        self.ui.ButtonRstSerie.clicked.connect(self.resetPuertosSeries)
        self.ui.pushButton_12.clicked.connect(self.nuevoProyecto)
        self.ui.pushButton_13.clicked.connect(self.eliminarProyecto)
        self.ui.pushButton_11.clicked.connect(self.solicitaAyuda)
        

    def iniciaBotonesRx(self):
        self.ui.pushButton_20.setEnabled(False)
        self.ui.pushButton_21.setEnabled(False)
        self.ui.pushButton_10.setEnabled(False)
        self.ui.pushButton_7.setEnabled(False)
        

    def solicitaAyuda(self):
        ayuda(self).exec_()


    def listaAplicativos(self):
        os.system('ls IcECIAA/aplicativos/ > aplicativos.lst')
        self.aplicativo='ninguno'
        fp= open('aplicativos.lst', 'r')
        lista = list(fp)
        fp.close
        os.system('rm aplicativos.lst')
        self.ui.comboBox_8.addItem('ninguno')
        for elemento in lista:
            self.ui.comboBox_8.addItem(str(elemento).strip())
        self.ui.comboBox_8.setCurrentIndex(0)
        self.ui.comboBox_8.currentTextChanged.connect(self.aplicativoSeleccionado)
        
    def aplicativoSeleccionado(self):
        #perceptron(self).exec_()
        self.habilitaTerminalRx= False
        memoriaSD(self).exec_()
             
             
        
    def nuevoProyecto(self):
        self.proyectoCreado=''
        nvoProyecto(self).exec_()
        if self.proyectoCreado is 'creado':
             self.ui.comboBox.addItem(str(self.proyecto))


    def eliminarProyecto(self):
        self.respuesta = ''
        self.texto='¿ Desea eliminar el Proyecto: '+str(self.proyecto)+' ?'
        alerta(self).exec_()
        if self.respuesta is 'acepto':
            self.mensaje('borrando Proyecto '+str(self.proyecto)+'...')
            instante = datetime.now()
            proyectoOculto='mv Proyectos/'+str(self.proyecto)+'  Proyectos/.'+str(self.proyecto)+'_'+str(instante.year)+'_'+str(instante.month)+'_'+str(instante.day)+'_'+str(instante.hour)+'_'+str(instante.minute)
            os.system(proyectoOculto)
            index = self.ui.comboBox.findText(self.proyecto, QtCore.Qt.MatchFixedString)
            self.ui.comboBox.removeItem(index)
            self.mensaje(proyectoOculto)
            #self.ocultarProyecto()


        
    def graficador(self, parent=None, width=10, height=4, dpi=30):
        self.ui.graphicsView.setXRange(self.buffer, 0.05)
        #self.ui.graphicsView.setYRange(self.buffer, 0.05)

        if self.q.logitud() < self.buffer:
            self.iniPlot()
        self.thPloteo.daemon=True
        self.thPloteo.start()       
        
        
    def activaTerminalRx(self):
        self.thTerminalRx.daemon=True
        self.thTerminalRx.start()     
        
        
        
    def soloNumeros(self, dato):
        ndato=""
        for i in dato:
            if (i >= chr(48)) and (i <= chr(57)):
                ndato += i
        return(ndato)
        
    def loop(self):
        while 1:
            self.refreshPlot()

 
    def iniPlot(self):
        """Valor Inicial: Todo el buffer a 0  """
        self.x = np.arange(self.inicio,int(self.buffer)+int(self.inicio),1)
        self.data = 0 
        for i in range(int(self.buffer)):
            self.q.encolar(self.data)
        self.msjTermRX("Muestra N    Valor")
        self.ploteo= self.ui.graphicsView.plot(self.x,self.q.items)
        QtGui.QApplication.processEvents()  #Obligo a realizar el ploteo y continuar


    def refreshPlot(self):
        if self.adquirirDatos is True:
            try:
                newinfo = self.conexionRx.leePuerto()
                canales=str(newinfo).split('|')
                cantCanalesRx= len(canales)
                if int(self.canalselRx) >= int(cantCanalesRx):
                    self.canalselRx=0
                    self.ui.label_6.setText("Canal "+str(self.canalselRx)+" Desactivado.")
                    self.ui.label_6.setStyleSheet("color: #000000; background-color: red;")  
                    self.ui.comboBox_6.setCurrentIndex(0)

                
                canalSeleccionado= str(canales[int(self.canalselRx)])
                newdata= int(re.sub("\D", "", canalSeleccionado))
                
                self.msjTermRX(str(self.inicio)+"......................"+str(newdata))
                self.DatosRecibidosRX.append(str(newdata))
           
                if int(self.q.logitud()) != self.buffer:
                    self.q.encolar(newdata)
                if int(self.q.logitud()) == self.buffer:
                    self.q.desencolar()
                    self.q.encolar(newdata)
                if self.q.logitud() == self.buffer:
                    fin= self.buffer+self.inicio
                    self.ui.graphicsView.setXRange(self.inicio,fin, 0.05)
                    self.ploteo.setData(list(self.x),self.q.items)
                    QtGui.QApplication.processEvents()  #Obligo a realizar el ploteo y continuar
                    self.x = np.arange(self.inicio,self.buffer+self.inicio,1)
                    self.inicio= self.inicio + 1
                else:
                    newdata = int( self.conexionRx.leePuerto())   #Se lee el puerto para vaciar el buffer
            except:
                mensaje="Se produjo algún error en la recepción de datos"
                self.msjTermRX(mensaje)
                self.DatosRecibidosRX.append(str(mensaje))
        
        if self.adquirirDatos is False:
            try:
                newdata = self.conexionRx.leePuerto()   #Se lee el puerto para vaciar el buffer
                self.datosPerdidos+=1
                if self.MsjSuspenderRX == 0:
                    mensaje= "Recepción CANCELADA. "+str(datetime.now())
                    self.msjTermRX(mensaje)
                    self.DatosRecibidosRX.append(str(mensaje))
                    self.MsjSuspenderRX = 1
            except:
                pass
 


    def graficadorRun(self):
        self.DatosRecibidosRX=list()
        self.adquirirDatos= True
        self.msjTermRX("Datos Perdidos: "+str(self.datosPerdidos))
        self.datosPerdidos=0
        self.ui.pushButton_20.setEnabled(True)
        self.ui.pushButton_21.setEnabled(False)
        self.ui.pushButton_10.setEnabled(False)
        if not self.thPloteo.is_alive() and str(self.puertoRx) == "open":
            self.graficador()
        self.ui.label_6.setText("Captura Iniciada")
        self.ui.label_6.setStyleSheet("color: #000000; background-color: green;")  
        pass
        

    def graficadorStop(self):
        self.ui.pushButton_10.setEnabled(True)
        self.ui.pushButton_20.setEnabled(False)
        self.ui.pushButton_21.setEnabled(True)
        if self.thPloteo.is_alive():
            self.adquirirDatos = False
            self.MsjSuspenderRX = 0
            self.ui.label_6.setText("Captura Detenida")
            self.ui.label_6.setStyleSheet("color: #000000; background-color: yellow;")
            pass


    def guardarDatosRX(self):
        import time
        if len(self.DatosRecibidosRX) > 1:
            self.mensaje("longitud: "+ str(len(self.DatosRecibidosRX)))
            fileRx= self.pathRx+'DatosRx'+str(time.strftime("%Y_%m_%d_%H_%M_%S"))+'.csv'
            self.fpRx= open(fileRx, 'w')
            for RXs in self.DatosRecibidosRX:
                self.fpRx.write(RXs+";\n")            
            self.fpRx.close()
            self.mensaje("*** Se han guardado los datos recibidos en "+str(fileRx))
            self.ui.label_6.setText("Captura Guardada")
            self.ui.label_6.setStyleSheet("color: #000000; background-color: blue;")
            self.ui.pushButton_21.setEnabled(False)


    def leerDatos(self):
        with open("/tmp/datos3.csv") as csvarchivo:
            entrada = pd.read_csv(csvarchivo,  sep=',', comment='#')
            datos= list()
            for reg in entrada:
                datos.append(reg)
                print("dato: "+reg) 
            return datos

    def Conexion(self):
        self.ui.pushButton.clicked.connect(self.compilar_M0)
        self.ui.pushButton_4.clicked.connect(self.escribirM0)
        self.ui.pushButton_2.clicked.connect(self.compilar_M4)
        self.ui.pushButton_3.clicked.connect(self.escribirM4)
        self.ui.pushButton_10.clicked.connect(self.graficadorRun)
        self.ui.pushButton_20.clicked.connect(self.graficadorStop)
        self.ui.pushButton_5.clicked.connect(self.aceptarTx)
        self.ui.pushButton_6.clicked.connect(self.cancelarTx)    
        self.ui.pushButton_8.clicked.connect(self.aceptarRx)
        self.ui.pushButton_7.clicked.connect(self.cancelarRx)    
        self.ui.pushButton_21.clicked.connect(self.guardarDatosRX)
        self.ui.pushButton_14.clicked.connect(self.abrirProyecto)
        app.aboutToQuit.connect(self.finalizarApp)
        
    
    def pintaBotonA(self):
        if not self.ui.lineEdit.text().strip(' ') == "" and self.puertoTx== "open":
            self.ui.ButtonTxA.setStyleSheet("background-color: green;")
            self.ui.ButtonTxA.setEnabled(True)
        else:
            self.ui.ButtonTxA.setStyleSheet("background-color: red;")
            self.ui.ButtonTxA.setEnabled(False)
    
    def pintaBotonB(self):
        if not self.ui.lineEdit_2.text().strip(' ') == "" and self.puertoTx== "open":
            self.ui.ButtonTxB.setStyleSheet("background-color: green;")
            self.ui.ButtonTxB.setEnabled(True)
        else:
            self.ui.ButtonTxB.setStyleSheet("background-color: red;")
            self.ui.ButtonTxB.setEnabled(False)

    def pintaBotonC(self):
        if not self.ui.lineEdit_3.text().strip(' ') == "" and self.puertoTx== "open":
            self.ui.ButtonTxC.setStyleSheet("background-color: green;")
            self.ui.ButtonTxC.setEnabled(True)
        else:
            self.ui.ButtonTxC.setStyleSheet("background-color: red;")
            self.ui.ButtonTxC.setEnabled(False)
            
    def pintaBotonD(self):
        if not self.ui.lineEdit_4.text().strip(' ') == "" and self.puertoTx== "open":
            self.ui.ButtonTxD.setStyleSheet("background-color: green;")
            self.ui.ButtonTxD.setEnabled(True)
        else:
            self.ui.ButtonTxD.setStyleSheet("background-color: red;")
            self.ui.ButtonTxD.setEnabled(False)
    
    def pintaBotonE(self):
        if not self.ui.lineEdit_5.text().strip(' ') == "" and self.puertoTx== "open":
            self.ui.ButtonTxE.setStyleSheet("background-color: green;")
            self.ui.ButtonTxE.setEnabled(True)
        else:
            self.ui.ButtonTxE.setStyleSheet("background-color: red;")
            self.ui.ButtonTxE.setEnabled(False)

    def pintaBotonF(self):
        if not self.ui.lineEdit_6.text().strip(' ') == "" and self.puertoTx== "open":
            self.ui.ButtonTxF.setStyleSheet("background-color: green;")
            self.ui.ButtonTxF.setEnabled(True)
        else:
            self.ui.ButtonTxF.setStyleSheet("background-color: red;")
            self.ui.ButtonTxF.setEnabled(False)

    def pintaBotonG(self):
        if not self.ui.lineEdit_7.text().strip(' ') == "" and self.puertoTx== "open":
            self.ui.ButtonTxG.setStyleSheet("background-color: green;")
            self.ui.ButtonTxG.setEnabled(True)
        else:
            self.ui.ButtonTxG.setStyleSheet("background-color: red;")
            self.ui.ButtonTxG.setEnabled(False)
            
    def pintaBotonH(self):
        if not self.ui.spinBox.value == 0 and self.puertoTx== "open":
            self.ui.ButtonTxH.setStyleSheet("background-color: green;")
            self.ui.ButtonTxH.setEnabled(True)
        else:
            self.ui.ButtonTxH.setStyleSheet("background-color: red;")
            self.ui.ButtonTxH.setEnabled(False)
            

    def pintaBotonScp(self):
        if not str(self.ui.comboBox_7.currentText()).strip() == 'none' and self.puertoTx== "open":
            self.ui.ButtonTxScp.setStyleSheet("background-color: green;")
            self.ui.ButtonTxScp.setEnabled(True)
        else:
            self.ui.ButtonTxScp.setStyleSheet("background-color: red;")
            self.ui.ButtonTxScp.setEnabled(False)
            
            
    
    def listaProyectos(self):
        os.system('ls Proyectos/ > proyectos.lst')
        self.proyecto='00-PRUEBA'
        fp= open('proyectos.lst', 'r')
        lista = list(fp)
        fp.close
        os.system('rm proyectos.lst')
        for elemento in lista:
            self.ui.comboBox.addItem(str(elemento).strip())
        self.ui.comboBox.setCurrentIndex(0)
        self.ui.comboBox.currentTextChanged.connect(self.proyectoSeleccionado)
        
        
    def listaScripts(self):
        os.system('ls IcECIAA/scripts/ > scripts.lst')
        fp= open('scripts.lst', 'r')
        listaScp = list(fp)
        fp.close
        os.system('rm scripts.lst')
        #self.ui.comboBox_7.addItem(str("Ninguno"))
        self.scriptsel='000.scp'
        for elemento in listaScp:
            self.ui.comboBox_7.addItem(str(elemento).strip())
        self.ui.comboBox_7.setCurrentIndex(0)
        self.ui.comboBox_7.currentTextChanged.connect(self.selecScript_clicked)

 
    def proyectoSeleccionado(self):
        self.proyecto = str(self.ui.comboBox.currentText()).strip()
        fp=open('Proyectos/'+str(self.proyecto)+'/Readme.txt', 'r')
        contenido=fp.read()
        self.ui.textBrowser_4.setText(contenido)
        fp.close()
        
    def abrirProyecto(self):
        ruta = 'Proyectos/'+str(self.proyecto)+'/'+str(self.proyecto)+'.cfg'
        if not os.path.exists(ruta):
            with open(ruta, 'w') as fp:
                fp.writelines('<?xml version="1.0" encoding="utf-8"?>\n<cfginterfaz>\n<nameProject>\n'+str(self.proyecto)+'\n</nameProject>\n</cfginterfaz>')
            self.mensaje("Se ha creado archivo de configuración "+str(self.proyecto)+'.cfg')
            fp.close
        doc = etree.parse(ruta)  # parseo el XML
        etree.tostring(doc,pretty_print=True ,xml_declaration=True, encoding="utf-8")
        raiz=doc.getroot()
        self.mensaje("leyendo..."+str(raiz[0].text).strip())
        #Abro el gedit con los archivos principales
        rutaArchivos = 'Proyectos/'+str(self.proyecto)+'/M0/src/main_m0.c  Proyectos/'+str(self.proyecto)+'/M4/src/main_m4.c'
        os.system('gedit '+rutaArchivos +'&')
        

    def mensaje(self, texto):
        self.ui.textBrowser_2.append(texto)

    def msjTermRX(self, texto):
        self.ui.textBrowser_3.append(texto)
        

#Genera listado de puertos serie existentes
    def listaPuertosSerie(self):
        self.puertoRx="close"
        self.puertoselRx=0
        self.puertoTx="close"
        self.puertoselTx=0
        os.system('ls /dev/ttyS* /dev/ttyA* /dev/ttyU* > puertos_disponibles.lst')
        self.fpRxTx= open('puertos_disponibles.lst', 'r')
        self.listadoPuertos = list(self.fpRxTx)
        self.fpRxTx.close
        os.system('rm puertos_disponibles.lst')
        
        
    def listaPuertosRx(self):
        self.ui.comboBox_5.addItem('none')
        for elemento in self.listadoPuertos:
            self.ui.comboBox_5.addItem(elemento.strip())
        self.ui.comboBox_5.setCurrentIndex(0)
        if not str(self.ui.comboBox_5.currentText()).strip() is 'none':
            self.ui.comboBox_5.currentTextChanged.connect(self.selecPuertosRx_clicked )


    def selecPuertosRx_clicked(self):
        self.puertoselRx = str(self.ui.comboBox_5.currentText()).strip()
        self.mensaje("*** Ha seleccionado el Puerto Serie: "+str(self.puertoselRx))


#Genera listado de canales
    def listaCanalesRx(self):
        canalesRx = ['0','1','2','3','4','5','6','7','8','9']
        self.canalselRx=0
        for elemento in canalesRx:
            self.ui.comboBox_6.addItem(elemento)
        self.ui.comboBox_6.setCurrentIndex(0)
        self.ui.comboBox_6.currentTextChanged.connect(self.selecCanalRx_clicked)

    def selecCanalRx_clicked(self):
        self.canalselRx = str(self.ui.comboBox_6.currentText()).strip()
        self.ui.textBrowser_2.append("Se ha configurado el Canal: "+ str(self.canalselRx))
        self.ui.label_6.setText("Canal "+str(self.canalselRx)+" Activado")
        self.ui.label_6.setStyleSheet("color: #000000; background-color: green;")
        
#Genera listado de tasa de transferencia del puerto 
    def listaBaudiosRx(self):
        listaRx = ['2400','4800','9600','19200','38400','57600','115200']
        self.baudiosselRx=9600
        for elemento in listaRx:
            self.ui.comboBox_4.addItem(elemento)
        self.ui.comboBox_4.setCurrentIndex(2)
        self.ui.comboBox_4.currentTextChanged.connect(self.selecBaudiosRx_clicked)


    def selecBaudiosRx_clicked(self):
        self.baudiosselRx = str(self.ui.comboBox_4.currentText()).strip()
        self.ui.textBrowser_2.append("Se ha configurado para Puerto Receptor: "+ str(self.baudiosselRx)+" baudios")
        


    def resetPuertosSeries(self):
        try:
            self.eliminarPuertos()
            self.listaPuertosSerie()
            self.resetRx()
            self.resetTx()
            self.listaPuertosRx()
            self.listaPuertosTx()
            try:
                self.conexionRx.cierraPuerto()
                self.mensaje('Puerto Serie Rx: CERRADO')
            except:
                pass
            try:
                self.conexionTx.cierraPuerto()
                self.mensaje('Puerto Serie Tx: CERRADO')
            except:
                pass
            
            self.mensaje("Se han reseteados los Puertos Serie.")
        except:
            self.mensaje("No se han podido resetear los Puertos Serie.")
            pass
#        listaScp= ['none', 'prueba.scp', 'dataToSDCard.scp' ]
#        self.scriptsel='none'
#        for elemento in listaScp:
#            self.ui.comboBox_7.addItem(elemento)
#        self.ui.comboBox_7.setCurrentIndex(1)
#        self.ui.comboBox_7.currentTextChanged.connect(self.selecScript_clicked)


    def selecScript_clicked(self):
        self.scriptsel = str(self.ui.comboBox_7.currentText()).strip()
        self.ui.textBrowser_2.append("Se ha seleccioado el script: "+ str(self.scriptsel)+" para ser ejecutado")
        try:
            fpScript=open('IcECIAA/scripts/'+ str(self.scriptsel), 'r')
            contenido=fpScript.readlines()
            texto=""
            limiteTexto= 60
            for linea in contenido:
                if len(linea) >= limiteTexto:
                    linea_0=linea[0:int(limiteTexto)]
                    linea_0+= "\n"
                    texto+= linea_0 + linea[int(limiteTexto):len(linea)]
                else:
                    texto += linea
            self.ui.textEdit.clear()
            self.ui.textEdit.append(texto)
            fpScript.close()
        except:
             self.ui.textBrowser_2.append("No se ha podido abrir el archivo "+ str(self.scriptsel))


    def guardarScript(self):
        contenidoScript= self.ui.textEdit.toPlainText()
        try:   
            with open('IcECIAA/scripts/'+ str(self.scriptsel), 'w') as f:
                f.write(str(contenidoScript))
            f.close()
            self.mensaje('*** Se han guardado las instrucciones en IcECIAA/scripts/'+ str(self.scriptsel))
            self.ui.label_6.setText("Script Guardado")
            self.ui.label_6.setStyleSheet("color: #000000; background-color: gray;")
        except:
            self.mensaje('*** No se han podido guardar las instrucciones en IcECIAA/scripts/'+ str(self.scriptsel)+ ' Contenido: '+ str(contenidoScript))


    def terminalRx(self):
        import time
        self.ui.textBrowser_3.append('Leyendo datos del puerto serie...')
        contador=0
        texto='hola'
        while True:
            if self.habilitaTerminalRx:
                texto= self.conexionRx.leePuerto()  # str(texto) +' '+ str(contador) 
                self.ui.textBrowser_3.append(str(texto))
                contador+= 1
                time.sleep(0.003)
                self.ui.textBrowser_3.verticalScrollBar().setValue(self.ui.textBrowser_3.verticalScrollBar().maximum())
                QtGui.QApplication.processEvents()  #Obligo a realizar la orden gráfica y continuar
            else:
                time.sleep(3)
                self.ui.textBrowser_3.append('Recepción de datos cancelada...')
        self.ui.textBrowser_3.append('Proceso Recepción Finalizado')


    def aceptarRx(self):
        self.adquirirDatos= False
        if str(self.puertoselRx).strip() == str(self.puertoselTx).strip() and str(self.puertoTx)=="open":
            self.mensaje("*** El Puerto "+str(self.puertoselTx)+" ya se encuentra abierto con una Tasa de Transferencia de "+str(self.baudiosselTx)+" baudios")
        elif str(self.puertoselRx).strip() == str(self.puertoselTx).strip() and str(self.puertoTx)=="close":
            self.mensaje("*** Debe seleccionar un Puerto Serie VALIDO")
        elif str(self.puertoselRx).strip() != 0 and str(self.puertoRx)=="open":
            self.mensaje("*** El Puerto Serie "+str(self.puertoselRx)+" ya se encuentra ABIERTO")
        else:
            if str(self.puertoselRx) != 0 and str(self.puertoRx) != "open":
                try:
                    self.conexionRx =SerialCom(self.puertoselRx, self.baudiosselRx)
                    self.puertoRx= "open"
                    self.mensaje("*** Puerto Receptor: "+str(self.puertoselRx)+" velocidad: "+str(self.baudiosselRx)+" baudios")
                    self.ui.pushButton_8.setEnabled(False)
                    self.ui.pushButton_7.setEnabled(True)
                    self.ui.comboBox_4.setEnabled(False)
                    self.ui.comboBox_5.setEnabled(False)
                    self.ui.pushButton_10.setEnabled(True)
                    self.ui.pushButton_20.setEnabled(False)
                    self.ui.pushButton_21.setEnabled(False)
                    
                    self.habilitaTerminalRx= True
                    if self.ui.checkBox.isChecked():
                        self.graficador()
                    else:
                        self.activaTerminalRx()
#                    self.thPloteo = threading.Thread(target=self.loop)
#                    self.thPloteo.daemon=True
#                    if not self.thPloteo.is_alive():
#                        self.thPloteo.start()    
                except:
                    self.mensaje("*** No se puede abrir el Puerto Receptor: "+str(self.puertoselRx))
                    pass
                
    def cancelarRx(self):
        try:
            if self.puertoselRx == self.puertoselTx:
                self.resetRx()
                self.resetTx()
#                self.conexionRx.cierraPuerto()
#                self.conexionTx.cierraPuerto()
#                self.ui.pushButton_6.setEnabled(False)
#                self.ui.pushButton_7.setEnabled(False)
            else:
#                self.conexionRx.cierraPuerto()
                self.mensaje("*** Puerto Receptor: "+str(self.puertoselRx)+" CERRADO")
                self.puertoselRx=0
                self.resetRx()
#            self.ui.pushButton_8.setEnabled(True)
#            self.ui.pushButton_7.setEnabled(False)
#            self.ui.comboBox_4.setEnabled(True)
#            self.ui.comboBox_5.setEnabled(True)
#            self.ui.pushButton_10.setEnabled(False)
#            self.ui.pushButton_20.setEnabled(False)
#            self.ui.pushButton_21.setEnabled(False)
        except:
            self.mensaje("*** No se ha podido CERRAR el Puerto Receptor: "+str(self.puertoselRx))
            pass

    def resetRx(self):
        self.puertoRx="close"
        #self.conexionRx.cierraPuerto()
        self.ui.pushButton_7.setEnabled(False)  #Deshabilita 'Cancelar'
        self.ui.comboBox_4.setEnabled(True)     #HAbilita BAUDIOS
        self.ui.comboBox_5.setEnabled(True)     #Habilita Puertos Rx
        self.ui.pushButton_8.setEnabled(True)   #Habilita Aceptar 
        self.ui.pushButton_10.setEnabled(False) #Deshabilita 'Iniciar Captura'
        self.ui.pushButton_20.setEnabled(False) #Deshabilita 'Detener Captura'
        self.ui.pushButton_21.setEnabled(False) #Deshabilita 'Guardar Captura'

#Fin de GRAFICADOR

#Transmisor de DATOS:
#Genera listado de puertos serie existentes
    def listaPuertosTx(self):
        self.ui.comboBox_2.addItem('none')
        for elemento in self.listadoPuertos:
            self.ui.comboBox_2.addItem(elemento.strip())
        self.ui.comboBox_2.setCurrentIndex(0)
        if not str(self.ui.comboBox_5.currentText()).strip() is 'none':
            self.ui.comboBox_2.currentTextChanged.connect(self.selecPuertosTx_clicked )
        
        
    def eliminarPuertos(self):
        self.ui.comboBox_2.clear()
        self.ui.comboBox_5.clear()


    def selecPuertosTx_clicked(self):
        self.puertoselTx = str(self.ui.comboBox_2.currentText()).strip()
        self.mensaje("*** Ha seleccionado el Puerto Serie: "+str(self.puertoselTx))
        
        
#Genera listado de tasa de transferencia del puerto
    def listaBaudiosTx(self):
        lista = ['2400','4800','9600','19200','38400','57600','115200']
        self.baudiosselTx=9600
        for elemento in lista:
            self.ui.comboBox_3.addItem(elemento)
        self.ui.comboBox_3.setCurrentIndex(2)
        self.ui.comboBox_3.currentTextChanged.connect(self.selecBaudiosTx_clicked )


    def selecBaudiosTx_clicked(self):
        self.baudiosselTx = str(self.ui.comboBox_3.currentText()).strip()
        self.ui.textBrowser_2.append("Se ha configurado para Puerto Transmisor:"+ str(self.baudiosselTx)+" baudios")

    def procesaDatoTx(self, texto):
        out= texto.split('$')
        out2= out[1].split('#')
        salida= int(out[0])
        self.conexionTx.escribeDatoPuerto(salida.to_bytes(4, byteorder='big'))
        salida= int(out2[0])
        self.conexionTx.escribeDatoPuerto(salida.to_bytes(4, byteorder='big'))
        return int(salida)


    def datoA_Tx(self):
        import time
        texto= self.ui.lineEdit.text()
        self.mensaje("Se envió por A: "+str(texto))
        self.ui.label_5.setText("Enviando... "+str(texto))
        self.ui.label_5.setStyleSheet("color: #FFFFFF; background-color: green;")
        self.procesaDatoTx(texto)
#        self.conexionTx.escribePuerto(texto)
        QtGui.QApplication.processEvents()
        time.sleep(self.delayBotonesTx)
        self.ui.label_5.setText("Enviado: "+str(texto))
        self.ui.label_5.setStyleSheet("color: #000000; background-color: red;")

    def datoB_Tx(self):
        import time
        texto= self.ui.lineEdit_2.text()
        self.mensaje("Se envió por B: "+str(texto))
        self.ui.label_5.setText("Enviando... "+str(texto))
        self.ui.label_5.setStyleSheet("color: #FFFFFF; background-color: green;")
        self.procesaDatoTx(texto)
#        self.conexionTx.escribePuerto(texto)
        QtGui.QApplication.processEvents()
        time.sleep(self.delayBotonesTx)
        self.ui.label_5.setText("Enviado: "+str(texto))
        self.ui.label_5.setStyleSheet("color: #000000; background-color: red;")
        
    def datoC_Tx(self):
        import time
        texto= self.ui.lineEdit_3.text()
        self.mensaje("Se envió por C: "+str(texto))
        self.ui.label_5.setText("Enviando... "+str(texto))
        self.ui.label_5.setStyleSheet("color: #FFFFFF; background-color: green;")
        self.procesaDatoTx(texto)
#        self.conexionTx.escribePuerto(texto)
        QtGui.QApplication.processEvents()
        time.sleep(self.delayBotonesTx)
        self.ui.label_5.setText("Enviado: "+str(texto))
        self.ui.label_5.setStyleSheet("color: #000000; background-color: red;")

    def datoD_Tx(self):
        import time
        texto= self.ui.lineEdit_4.text()
        self.mensaje("Se envió por D: "+str(texto))
        self.ui.label_5.setText("Enviando... "+str(texto))
        self.ui.label_5.setStyleSheet("color: #FFFFFF; background-color: green;")
        self.procesaDatoTx(texto)
#        self.conexionTx.escribePuerto(texto)
        QtGui.QApplication.processEvents()
        time.sleep(self.delayBotonesTx)
        self.ui.label_5.setText("Enviado: "+str(texto))
        self.ui.label_5.setStyleSheet("color: #000000; background-color: red;")
        

    def datoE_Tx(self):
        import time
        texto= self.ui.lineEdit_5.text()
        self.mensaje("Se envió por E: "+str(texto))
        self.ui.label_5.setText("Enviando... "+str(texto))
        self.ui.label_5.setStyleSheet("color: #FFFFFF; background-color: green;")
        self.procesaDatoTx(texto)
#        self.conexionTx.escribePuerto(texto)
        QtGui.QApplication.processEvents()
        time.sleep(self.delayBotonesTx)
        self.ui.label_5.setText("Enviado: "+str(texto))
        self.ui.label_5.setStyleSheet("color: #000000; background-color: red;")
        
    def datoF_Tx(self):
        import time
        texto= self.ui.lineEdit_6.text()
        self.mensaje("Se envió por F: "+str(texto))
        self.ui.label_5.setText("Enviando... "+str(texto))
        self.ui.label_5.setStyleSheet("color: #FFFFFF; background-color: green;")
        self.procesaDatoTx(texto)
#        self.conexionTx.escribePuerto(texto)
        QtGui.QApplication.processEvents()
        time.sleep(self.delayBotonesTx)
        self.ui.label_5.setText("Enviado: "+str(texto))
        self.ui.label_5.setStyleSheet("color: #000000; background-color: red;")
        self.ui.comboBox_7. spinBox.value()
    def datoG_Tx(self):
        import time
        texto= self.ui.lineEdit_7.text()
        self.mensaje("Se envió por G: "+str(texto))
        self.ui.label_5.setText("Enviando... "+str(texto))
        self.ui.label_5.setStyleSheet("color: #FFFFFF; background-color: green;")
        self.procesaDatoTx(texto)
#        self.conexionTx.escribePuerto(texto)
        QtGui.QApplication.processEvents()
        time.sleep(self.delayBotonesTx)
        self.ui.label_5.setText("Enviado: "+str(texto))
        self.ui.label_5.setStyleSheet("color: #000000; background-color: red;")

    def datoH_Tx(self):
        import time
        texto= self.ui.spinBox.value()
        texto= '44$'+str(texto)+'#'
        self.mensaje("Se envió por H: "+str(texto))
        self.ui.label_5.setText("Enviando... "+str(texto))
        self.ui.label_5.setStyleSheet("color: #FFFFFF; background-color: green;")
        self.procesaDatoTx(texto)
#        self.conexionTx.escribePuerto(str(texto))
        QtGui.QApplication.processEvents()
        time.sleep(self.delayBotonesTx)
        self.ui.label_5.setText("Enviado: "+str(texto))
        self.ui.label_5.setStyleSheet("color: #000000; background-color: red;")

        
    def datoScp_Tx(self):
        import time
        try:
            with open('IcECIAA/scripts/' + self.scriptsel, 'r') as lineas:
                contador=1
                for linea in lineas:
                    linea.replace('\n\r', '')
                    if linea.strip():
                        if linea[0] is not str('/') and linea[1] is not str('/'):
                        #caracter= ' // '
                        #linea= linea + caracter
                            comando= linea.split('//')
                            if comando[0] is not '':
                                self.procesaDatoTx(str(comando[0]))
#                                self.conexionTx.escribePuerto(str(comando))
                                QtGui.QApplication.processEvents()
                                self.mensaje("Contenido: " + comando[0])
#                                time.sleep(0.35)
                    contador= contador + 1
        except:
            self.mensaje("No se ha podido leer el Script: "+ self.scriptsel +' en línea: '+ str(contador))
        
    def aceptarTx(self):
        if str(self.puertoselRx).strip() == str(self.puertoselTx).strip() and str(self.puertoRx)=="open":
            self.mensaje("*** El Puerto "+self.puertoselRx+" ya se encuentra abierto con una Tasa de Transferencia de "+str(self.baudiosselRx)+" baudios")
            self.conexionTx= self.conexionRx
            self.ui.pushButton_6.setEnabled(True)
            self.ui.pushButton_5.setEnabled(False)
            self.ui.comboBox_2.setEnabled(False)
            self.ui.comboBox_3.setEnabled(False)            
            self.puertoTx= "open"
            self.mensaje("Puerto de Tarnsmisión/Recepción: "+str(self.puertoselTx))
        elif str(self.puertoselRx).strip() == str(self.puertoselTx).strip() and str(self.puertoRx)=="close":
            self.mensaje("*** Debe seleccionar un Puerto Serie VALIDO")
        elif str(self.puertoselTx).strip() != 0 and str(self.puertoTx)=="open":
            self.mensaje("*** El Puerto Serie "+str(self.puertoselTx)+" ya se encuentra ABIERTO")
        else:
            if str(self.puertoselTx) != 0 and str(self.puertoTx) != "open":
                try:
                    self.conexionTx =SerialCom(self.puertoselTx, self.baudiosselTx)
                    self.puertoTx= "open"
                    self.mensaje("*** Puerto Transmisor: "+str(self.puertoselTx)+" velocidad: "+str(self.baudiosselTx)+" baudios")
                    self.ui.pushButton_6.setEnabled(True)
                    self.ui.pushButton_5.setEnabled(False)
                    self.ui.comboBox_2.setEnabled(False)
                    self.ui.comboBox_3.setEnabled(False)
                except:
                    self.mensaje("*** No se puede abrir el Puerto Transmisor: "+str(self.puertoselTx))
                    pass
                
    def cancelarTx(self):
        try:
            if self.puertoselRx == self.puertoselTx:
                self.resetRx()
                self.resetTx()
#                self.conexionRx.cierraPuerto()
#                self.conexionTx.cierraPuerto()
#                self.ui.pushButton_6.setEnabled(False)
#                self.ui.pushButton_7.setEnabled(False)
            else:
#                self.conexionTx.cierraPuerto()
                self.mensaje("*** Puerto Transmisor: "+str(self.puertoselTx)+" CERRADO")
                self.puertoselTx=0
                self.puertoTx="close"
                self.resetTx()
#            self.ui.pushButton_5.setEnabled(True)
#            self.ui.pushButton_6.setEnabled(False)
#            self.ui.comboBox_2.setEnabled(True)
#            self.ui.comboBox_3.setEnabled(True)
        except:
            self.mensaje("*** No se ha podido CERRAR el Puerto Transmisor: "+str(self.puertoselTx))
            pass

    def resetTx(self):
        self.puertoTx="close"
        #self.conexionTx.cierraPuerto()
        self.ui.pushButton_6.setEnabled(False)  #Deshabilita 'Cancelar'
        self.ui.comboBox_3.setEnabled(True)     #HAbilita BAUDIOS
        self.ui.comboBox_2.setEnabled(True)     #Habilita Puertos Rx
        self.ui.pushButton_5.setEnabled(True)   #Habilita Aceptar 

        
#Fin de TRANSMISOR
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++      
# Operaciones de Compilación y Grabado
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    def compilar_M0(self):
        self.procesoM0= threading.Event()
        tarea_CompilaM0 = threading.Thread(target=self.compilarM0)
        tarea_CompilaM0.setDaemon(True)
        tarea_CompilaM0.start()
        self.flag_exit=3
        while self.flag_exit <= 5:
            if tarea_CompilaM0.isAlive()== False:
                self.detalle("Compilando "+ str(self.proyecto) +" -> M0 ")
                self.procesoM0.clear()
        
    def compilarM0(self):
        operacion1= os.system('make PROJECT=Proyectos/'+ str(self.proyecto) +'/M0 TARGET=lpc4337_m0 clean > salida.openocd 2>&1')
        operacion2= os.system('make PROJECT=Proyectos/'+ str(self.proyecto) +'/M0 TARGET=lpc4337_m0 PROJECT_INC_FOLDERS=Proyectos/'+str(self.proyecto)+'/codes > salida.openocd 2>&1')
        self.procesoM0.set()
        
    def compilar_M4(self):
        self.procesoM4= threading.Event()
        tarea_CompilaM4 = threading.Thread(target=self.compilarM4)
        tarea_CompilaM4.setDaemon(True)
        tarea_CompilaM4.start()
        self.flag_exit=3
        while self.flag_exit <= 5:
            if tarea_CompilaM4.isAlive()== False:
                self.detalle("Compilando "+ str(self.proyecto) +" -> M4 ")
                self.procesoM4.clear()
        
    def compilarM4(self):
        operacion1= os.system('make PROJECT=Proyectos/'+ str(self.proyecto) +'/M4 TARGET=lpc4337_m4 clean > salida.openocd 2>&1')
        operacion2= os.system('make PROJECT=Proyectos/'+ str(self.proyecto) +'/M4 TARGET=lpc4337_m4 PROJECT_INC_FOLDERS=Proyectos/'+str(self.proyecto)+'/codes > salida.openocd 2>&1')
        self.procesoM4.set()


    def detalle(self, leyenda):
        import time
        horaActual= time.strftime("%Y_%m_%d_%H_%M_%S")
        self.ui.textBrowser.append(leyenda + str(horaActual))
        fp=open('salida.openocd','r')
        resultado=fp.readlines()
        self.ui.textBrowser_2.clear()
        errores=0
        warnings=0
        for linea in resultado:
            self.ui.textBrowser_2.setTextBackgroundColor(QtGui.QColor('white') )
            if 'error' in linea:
                errores+=1
                self.ui.textBrowser_2.setTextBackgroundColor(QtGui.QColor('red') )
            if 'warning' in linea:
                warnings+=1
                self.ui.textBrowser_2.setTextBackgroundColor(QtGui.QColor('yellow') )
            self.ui.textBrowser_2.append(linea)
        self.ui.textBrowser_2.setTextColor(QtGui.QColor('blue'))
        self.ui.textBrowser_2.append("Finalizado. "+str(time.strftime("%Y_%m_%d_%H_%M_%S")))
        self.ui.textBrowser_2.append("Warnings: "+str(warnings)+" - Errores: "+str(errores))
        self.flag_exit= 7
        
    def escribirM0(self):
        operacion1= os.system('make PROJECT=Proyectos/'+str(self.proyecto)+'/M0 TARGET=lpc4337_m0 download > salida_escritura.openocd 2>&1')
        self.detalle("Grabación "+ str(self.proyecto) +" -> M0: finalizada ")
        
    def escribirM4(self):
        operacion1= os.system('make PROJECT=Proyectos/'+str(self.proyecto)+'/M4 TARGET=lpc4337_m4 download > salida_escritura.openocd 2>&1')
        self.detalle("Grabación "+ str(self.proyecto) +" -> M4: finalizada ")


class ayuda(QDialog) :

    def __init__(self, parent= None):
        from PyQt5.QtGui import QPixmap
        super(ayuda, self).__init__()
        self.ventanaAyuda = Ui_ayuda()
        self.ventanaAyuda.setupUi(self) 
        self.parent= parent
        pixmap = QPixmap('IcECIAA/imagenes/ice.png')
        self.ventanaAyuda.label_2.setPixmap(pixmap)
        fp= open('IcECIAA/config/Ayuda.gen', 'r')
        texto= fp.read()
        fp.close()
        #self.ventanaAyuda.textBrowser.setText(str(texto))
        self.ventanaAyuda.textBrowser.setOpenExternalLinks(True)
        self.ventanaAyuda.textBrowser.setHtml(texto);
        self.ventanaAyuda.pushButton.clicked.connect(self.cerrarAyuda)

    def cerrarAyuda(self):
        self.setWindowFlags(Qt.FramelessWindowHint | Qt.Popup)
        
        
        

class alerta(QDialog) :

    def __init__(self, parent= None):
        super(alerta, self).__init__()
        self.ventanaAlerta = Ui_alerta()
        self.ventanaAlerta.setupUi(self) 
        self.parent= parent
        self.ventanaAlerta.label.setText(str(self.parent.texto))
        self.ventanaAlerta.pushButton.clicked.connect(self.aceptaAlerta)
        self.ventanaAlerta.pushButton_2.clicked.connect(self.cancelaAlerta)

    def aceptaAlerta(self):
        self.parent.respuesta= 'acepto'
        self.setWindowFlags(Qt.FramelessWindowHint | Qt.Popup)
        
    def cancelaAlerta(self):
        self.parent.respuesta= ''
        self.setWindowFlags(Qt.FramelessWindowHint | Qt.Popup)
        





class nvoProyecto(QDialog) :

    def __init__(self, parent= None):
        super(nvoProyecto, self).__init__()
        self.ventanaProyecto = Ui_nvoProyecto()
        self.ventanaProyecto.setupUi(self) 
        self.parent= parent
        self.ventanaProyecto.pushButton_2.clicked.connect(self.crearNuevoProyecto)
        self.ventanaProyecto.pushButton.clicked.connect(self.cancelarNuevoProyecto)
        ruta= 'IcECIAA/config/Readme.gen'
        fp= open(ruta, 'r')
        self.ventanaProyecto.textEdit.setText(fp.read())
        fp.close()
        ruta= 'IcECIAA/config/EncabezadoM0.gen'
        fp= open(ruta, 'r')
        self.ventanaProyecto.textEdit_2.setText(fp.read())
        fp.close()
        ruta= 'IcECIAA/config/EncabezadoM4.gen'
        fp= open(ruta, 'r')
        self.ventanaProyecto.textEdit_3.setText(fp.read())
        fp.close()
        
    def completaDigitos(numero):
        if int(numero) < 10:
            return '0'+ numero
        

    def crearNuevoProyecto(self):
        import os.path as path
        nombreProyecto= self.ventanaProyecto.lineEdit.text()
        nombreProyecto= nombreProyecto.upper()
        if not path.exists('Proyectos/'+ str(nombreProyecto)):
            try:
                os.mkdir('Proyectos/'+str(nombreProyecto))
                os.mkdir('Proyectos/'+str(nombreProyecto)+'/M0')
                self.parent.mensaje('Se creó la carpeta M0')
                os.mkdir('Proyectos/'+str(nombreProyecto)+'/M4')
                self.parent.mensaje('Se creó la carpeta M4')
                os.mkdir('Proyectos/'+str(nombreProyecto)+'/M0/src')
                self.parent.mensaje('Se creó la carpeta M0/src')  
                os.mkdir('Proyectos/'+str(nombreProyecto)+'/M4/src')
                self.parent.mensaje('Se creó la carpeta M4/src')
                os.mkdir('Proyectos/'+str(nombreProyecto)+'/informacion')
                self.parent.mensaje('Se creó la carpeta informacion')
                os.system('cp IcECIAA/config/Makefile.gen Proyectos/'+str(nombreProyecto)+'/M0/Makefile' )
                self.parent.mensaje('Se creó M0/Makefile')
                os.system('cp IcECIAA/config/Makefile.gen Proyectos/'+str(nombreProyecto)+'/M4/Makefile' )
                self.parent.mensaje('Se creó la carpeta M4/Makefile')
                os.system('cp -R IcECIAA/config/codes Proyectos/'+str(nombreProyecto) )
                self.parent.mensaje('Se copiaron los archivos de la carpeta codes')
                instante = datetime.now()
                fechaCreacion= instante.strftime("%d/%m/%Y %H:%M") 

                
                texto= self.ventanaProyecto.textEdit_2.toPlainText()
                destino= 'Proyectos/'+str(nombreProyecto)+'/M0/src/main_m0.c'
                self.fp= open(destino, 'w')
                self.fp.write('/*\n*** Autor: '+ str(self.ventanaProyecto.lineEdit_2.text()) + '\n')
                self.fp.write('*** Fecha: '+str(fechaCreacion)+'\n')            
                self.fp.write('\n '+ str(texto) +'\n*/')
                self.fp.close()
                
                texto= self.ventanaProyecto.textEdit_3.toPlainText()
                destino= 'Proyectos/'+str(nombreProyecto)+'/M4/src/main_m4.c'
                self.fp= open(destino, 'w')
                self.fp.write('/*\n*** Autor: '+ str(self.ventanaProyecto.lineEdit_2.text()) + '\n')
                self.fp.write('*** Fecha: '+str(fechaCreacion)+'\n')
                self.fp.write('\n '+ str(texto) +'\n*/')
                self.fp.close()
                
                texto= self.ventanaProyecto.textEdit.toPlainText()
                destino= 'Proyectos/'+str(nombreProyecto)+'/Readme.txt'
                self.fp= open(destino, 'w')
                self.fp.write('\n*** Autor: '+ str(self.ventanaProyecto.lineEdit_2.text()))
                self.fp.write('\n*** Proyecto creado el: '+str(fechaCreacion)+'\n\n')
                self.fp.write(str(texto))
                self.fp.close()
                self.parent.proyecto= str(nombreProyecto)
                self.parent.proyectoCreado= 'creado'                
            except:
                pass
            self.parent.mensaje('Se creó nuevo Proyecto: '+ str(nombreProyecto))
        else:
            self.parent.mensaje('No se ha podido crear proyecto. El nombre: '+ str(nombreProyecto)+ ' ya fue utilizado. ')
        self.setWindowFlags(Qt.FramelessWindowHint | Qt.Popup)

    def cancelarNuevoProyecto(self):
        self.parent.mensaje('Operación Nuevo Proyecto: CANCELADA')
        self.setWindowFlags(Qt.FramelessWindowHint | Qt.Popup)
        
        


        
        
        

app = QApplication(sys.argv)
w = AppWindow()
w.show()
sys.exit(app.exec_())