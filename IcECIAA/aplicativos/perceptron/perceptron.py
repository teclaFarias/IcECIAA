#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Mar  6 09:35:05 2019

@author: tecla
"""

#from PyQt5.QtWidgets import QApplication, QMainWindow, QDockWidget, QDialog
#from vent_perceptron import Ui_perceptron
#

import subprocess
import re
from PyQt5.QtWidgets import QDialog
from aplicativos.perceptron.RNperceptron import RNPerceptron
from aplicativos.perceptron.vent_perceptron import Ui_perceptron
import time
from numpy import genfromtxt


class perceptron(QDialog) :

    def __init__(self, parent= None):
        super(perceptron, self).__init__()
        self.ventanaAplicacion = Ui_perceptron()
        self.ventanaAplicacion.setupUi(self) 
        self.parent= parent
        self.colorR=0
        self.colorV=0
        self.colorA=0
        self.archivo= 'IcECIAA/aplicativos/perceptron/training.csv'
        self.deshabilitaBotones()
        self.mensaje("*** Para comenzar a utilizar Pyceptron debe entrenar la Red Neuronal ***")
        self.conexion()
        self.show()
        
        
    def conexion(self):
        self.ventanaAplicacion.verticalSlider.valueChanged.connect(self.seteoRojo)
        self.ventanaAplicacion.verticalSlider_2.valueChanged.connect(self.seteoVerde)
        self.ventanaAplicacion.verticalSlider_3.valueChanged.connect(self.seteoAzul)
        self.ventanaAplicacion.pushButton.clicked.connect(self.verificarNeurona)
        self.ventanaAplicacion.pushButton_2.clicked.connect(self.entrenarNeurona)
        self.ventanaAplicacion.pushButton_3.clicked.connect(self.agregarPositivo)
        self.ventanaAplicacion.pushButton_4.clicked.connect(self.agregarNegativo)
        
        
    def deshabilitaBotones(self):
        self.ventanaAplicacion.pushButton_2.setEnabled(True)
        self.ventanaAplicacion.pushButton.setEnabled(False)
        self.ventanaAplicacion.pushButton_3.setEnabled(False)
        self.ventanaAplicacion.pushButton_4.setEnabled(False)
        self.ventanaAplicacion.verticalSlider.setEnabled(False)
        self.ventanaAplicacion.verticalSlider_2.setEnabled(False)
        self.ventanaAplicacion.verticalSlider_3.setEnabled(False)

    def habilitaBotones(self):
        self.ventanaAplicacion.pushButton.setEnabled(True)
        self.ventanaAplicacion.pushButton_2.setEnabled(True)
        self.ventanaAplicacion.pushButton_3.setEnabled(True)
        self.ventanaAplicacion.pushButton_4.setEnabled(True)
        self.ventanaAplicacion.verticalSlider.setEnabled(True)
        self.ventanaAplicacion.verticalSlider_2.setEnabled(True)
        self.ventanaAplicacion.verticalSlider_3.setEnabled(True)

        
    def seteoRojo(self):
        self.colorR= self.ventanaAplicacion.verticalSlider.value()
        self.ventanaAplicacion.label_3.setText("Rojo:"+str(self.colorR))      
        self.actualizaColor()

    def seteoVerde(self):
        self.colorV= self.ventanaAplicacion.verticalSlider_2.value()
        self.ventanaAplicacion.label_4.setText("Verde:"+str(self.colorV))      
        self.actualizaColor()
        
    def seteoAzul(self):
        self.colorA= self.ventanaAplicacion.verticalSlider_3.value()
        self.ventanaAplicacion.label_5.setText("Azul:"+str(self.colorA))      
        self.actualizaColor()
        

    def actualizaColor(self):
        #convierto de Decimal a Hexadecimal los colores
        rojoHex= "{0:x}".format(int(self.colorR))
        if len(rojoHex) == 1:
            rojoHex= "0"+rojoHex
        verdeHex= "{0:x}".format(int(self.colorV))
        if len(verdeHex) == 1:
            verdeHex= "0"+verdeHex
        azulHex= "{0:x}".format(int(self.colorA))
        if len(azulHex) == 1:
            azulHex= "0"+azulHex
            
        self.colorGenerado= rojoHex + verdeHex + azulHex
        
        self.ventanaAplicacion.label.setText("Color de 24 bits Generado [Hex]: #"+self.colorGenerado+"    [Decimal]: "+str(int(self.colorGenerado,16)))
        self.ventanaAplicacion.label_2.setText("Color Generado")
        self.ventanaAplicacion.label_2.setStyleSheet("color: #000000; background-color: #"+str(self.colorGenerado))

    def comandoSistema(self, comando):
        p = subprocess.Popen(comando, stdout=subprocess.PIPE, shell=True)
        texto=p.stdout.read()
        return str(re.sub("\D", "", str(texto)))


    def evaluarMulticapa(self):
        comando='./IcECIAA/aplicativos/perceptron/ptnMulticapa '+str(self.archivo)+' '+str(self.colorR)+' '+str(self.colorV)+' '+str(self.colorA)
        salida= self.comandoSistema(comando)
        time.sleep(0.5)
        #print (str(salida))
        if int(str(salida).strip())  is 1:
            self.ventanaAplicacion.label_8.setStyleSheet("color: #005F00; background-color: #F0F0F0")
            self.ventanaAplicacion.label_8.setText("SI")
        if int(str(salida).strip())  is 0:
            self.ventanaAplicacion.label_8.setStyleSheet("color: #F00000; background-color: #F0F0F0")
            self.ventanaAplicacion.label_8.setText("NO")

        
    def verificarNeurona(self):
        if self.ventanaAplicacion.checkBox.checkState():
            self.evaluarMulticapa()
        if self.pr.predict([int(self.colorR),int(self.colorV),int(self.colorA)]) == 1: 
            self.ventanaAplicacion.label_9.setStyleSheet("color: #005F00; background-color: #F0F0F0")
            self.ventanaAplicacion.label_9.setText("SI")
        else:
            self.ventanaAplicacion.label_9.setStyleSheet("color: #F00000; background-color: #F0F0F0")
            self.ventanaAplicacion.label_9.setText("NO")
        self.mensaje('Ultima verificación: '+str(self.archivo)+' '+str(self.colorR)+' '+str(self.colorV)+' '+str(self.colorA))


    def mensaje(self, texto):
        self.ventanaAplicacion.textBrowser.append(str(texto))


    def entrenarNeurona(self):
        try:
            input_data= genfromtxt(self.archivo, delimiter=' ')
    ## Creamos el perceptron
            self.pr = RNPerceptron(4,0.1) # Perceptron con 3 entradas X y 1 Y
            weights = [] # Lista con los pesos
            errors = []  # Lista con los errores
    ## Fase de entrenamiento
            for _ in range(10):
                for person in input_data:
                    output = person[-1]
                    inp = [1] + person[0:-1] # Agregamos un uno por default
                    weights.append(self.pr._w)
                    err = self.pr.train(inp,output)
                    errors.append(err)
            self.mensaje("Red Entrenada con "+str(self.archivo))
            self.habilitaBotones()
        except:
            self.mensaje("*** Archivo "+str(self.archivo)+" vacío o inexistente. Red sin información")
            self.habilitaBotones()
        

        
    def agregarPositivo(self):
        nuevoColor= str(self.colorR)+" "+str(self.colorV)+" "+str(self.colorA)+" 1"
        if self.escribeCSV(nuevoColor) is True:
            self.mensaje("Color: ("+str(self.colorR)+","+str(self.colorV)+","+str(self.colorA)+") --> AFIRMATIVO" )
        else:
            self.mensaje("No se pudo agregar el color: ("+str(self.colorR)+","+str(self.colorV)+","+str(self.colorA)+")")

    def agregarNegativo(self):
        nuevoColor= str(self.colorR)+" "+str(self.colorV)+" "+str(self.colorA)+" 0"
        if self.escribeCSV(nuevoColor) is True:
            self.mensaje("Color: ("+str(self.colorR)+","+str(self.colorV)+","+str(self.colorA)+") --> Negativo" )
        else:
            self.mensaje("No se pudo agregar el color: ("+str(self.colorR)+","+str(self.colorV)+","+str(self.colorA)+")")
            

    def escribeCSV(self, nuevoColor):
        try:
            fp= open(str(self.archivo), 'a')
            fp.write("\n"+str(nuevoColor))            
            fp.close()
            return True
        except:
            fp= open(str(self.archivo), 'w')
            fp.write("\n"+str(nuevoColor))            
            fp.close()
            return True
            
            
    def finalizarApp(self):
        self.cancelarRx()
        self.cancelarTx()            
        print('Finalizó correctamente')
        import sys
        sys.exit(0)        
        