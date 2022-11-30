#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Mar 11 11:51:29 2019

@author: tecla
"""

import pandas as pd
from PyQt5 import QtGui
import threading
from PyQt5.QtWidgets import QDialog
from PyQt5.QtWidgets import QApplication, QMessageBox, QMainWindow, QAction
from PyQt5.QtCore import Qt
from aplicativos.memoriaSD.vent_memoriaSD import Ui_memoriaSD
import os
#from vent_memoriaSD import Ui_memoriaSD


class memoriaSD(QDialog) :

    def __init__(self, parent= None):
        super(memoriaSD, self).__init__()
        self.ventanaMemoria = Ui_memoriaSD()
        self.ventanaMemoria.setupUi(self) 
        self.parent= parent
        self.habilitaTerminalMemSD= True
        self.ventanaMemoria.textBrowser.append('Recepción de datos habilitada...')
        self.thTerminalMemoria = threading.Thread(target=self.terminalMemoriaSD)
        self.datoRecibido=0
        self.textoCorregido=""
        self.conexionBotones()
        self.activaTerminalMemoria()
        self.listaImagenes()
        self.listaFuentes()
        self.listaIconos()
        self.listaDireccionesImagen()
        self.show()
        
    def conexionBotones(self):
        self.ventanaMemoria.pushButton.clicked.connect(self.inicializarMemoria)
        self.ventanaMemoria.pushButton_2.clicked.connect(self.leerBloquesMemoria)
        self.ventanaMemoria.pushButton_3.clicked.connect(self.leerElementoMemoria)
        self.ventanaMemoria.pushButton_4.clicked.connect(self.escribirElementoMemoria)
        self.ventanaMemoria.pushButton_5.clicked.connect(self.transferirBloque)
        self.ventanaMemoria.pushButton_6.clicked.connect(self.transferirFuente)
        self.ventanaMemoria.pushButton_7.clicked.connect(self.transferirIcono)        
        
#    def analizaBotonPixel0(self):
#        if int(self.col0 & (0x01)) is 1:
#            self.col0= self.col0 & (0xFE) 
#            self.ventanaMemoria.pushButtonPix0.setStyleSheet("background-color: white;")
#        else:
#            self.col0= self.col0 | (0x01) 
#            self.ventanaMemoria.pushButtonPix0.setStyleSheet("background-color: blue;")
#        self.actualizaValorColumnas()
#            
            


    def leerElementoMemoria(self):
#        import time
        adressSD=self.ventanaMemoria.lineEdit.text()
        valor= int(adressSD)
        comando= '50$'+str(valor)+'#'  # Seteo Address con valor de 32 bits
        self.parent.procesaDatoTx(str(comando))
        self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))
        valor= 1
        comando= '52$'+str(valor)+'#'  
        self.parent.procesaDatoTx(str(comando))
        self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))


    def escribirElementoMemoria(self):
        bloque=self.ventanaMemoria.lineEdit.text()
        valor= int(bloque)
        comando= '50$'+str(valor)+'#'  # Seteo Address con valor de 32 bits
        self.parent.procesaDatoTx(str(comando))
        self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))
        
        dato=self.ventanaMemoria.lineEdit_3.text()
        valor= int(dato)
        if valor > 255:
            valor=0
        comando= '53$'+str(valor)+'#'
        self.parent.procesaDatoTx(str(comando))
        self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))



    def transferirIcono(self):
        import time
        bloque= 0
        total=0
        colorR=0
        colorG=0
        colorB=0
        bloque= self.ventanaMemoria.comboBox_3.currentText()
        address= int(bloque)

        self.ventanaMemoria.textBrowser.append('Leyendo archivo...')
        contDatos=0
        rows = []
        rowsPixel= []
        nombreArchivo= self.ventanaMemoria.comboBox_4.currentText()
        pathImg= "IcECIAA/aplicativos/memoriaSD/imagenes/iconos/"+str(nombreArchivo)
        with open(pathImg, "rb") as fp:
            dato = fp.read(1)
            while dato != b"":
#                self.ventanaMemoria.textBrowser.append('Contenido: '+ str(ord(dato)))
                rowsPixel.append(ord(dato))
                dato = fp.read(1)
                
                
        if  int(len(rowsPixel)) == 0:
            self.ventanaMemoria.textBrowser.append('ERROR: Lectura de '+ str(len(rowsPixel))+' bytes')
            QtGui.QApplication.processEvents()
            return 0
        else:
            self.ventanaMemoria.textBrowser.append('Lectura de '+ str(len(rowsPixel))+' bytes')
            QtGui.QApplication.processEvents()


        cantidadHorizontalIconos= 4
        anchoIconoBytes= 60*3                      # anchoIconoBytes = 60 x 1pix(RGB) = 60 x 3
        altoPixelBytes= 60
        anchoTotalBytes= anchoIconoBytes * cantidadHorizontalIconos   # anchoTotalBytes = 180 x 4= 720
#        deltaHorizontal= anchoIconoBytes                 # deltaHorizontal = 180
#        deltaVertical= anchoTotalBytes * altoPixelBytes  # deltaVertical= 720 x 60= 43200
#        iconos= []
        pixelInicioIcono = 0
        deltaVertical = 0
        columnaIcono = 0

        for icono in range(0,20,1):
            pixelInicioIcono= int((columnaIcono * anchoIconoBytes) + deltaVertical)
                #Lectura de un Icono determinado
            for fila in range(0,altoPixelBytes,1):
                pixel=0
                for i in range(0,60,1):
                    colorR= int(rowsPixel[pixel + pixelInicioIcono])        # Obtengo R
                    pixel += 1
                    colorG= int(rowsPixel[pixel + pixelInicioIcono])    # Obtengo G
                    pixel += 1
                    colorB= int(rowsPixel[pixel + pixelInicioIcono])    # Obtengo B
                    pixel += 1
                    colorR= int(colorR & 0x00f8);
                    colorG1= int(colorG & 0xE0);
                    colorG2= int(colorG & 0x1C);
                    colorB= int(colorB & 0xf8);
                    ch1= (0xff)&( colorR | (colorG1 >> 5) );
                    ch2= (0xff)&((colorG2 << 3) | (colorB >> 3) );                
                    rows.append(ch1)
                    rows.append(ch2)
#                       total += 2
                        #fin de lectura de línea de icono
                pixelInicioIcono= pixelInicioIcono + (anchoIconoBytes * 4)  #paso a la siguiente línea del Icono
                
            columnaIcono += 1
            
                
#            self.ventanaMemoria.textBrowser.append('Icono: '+ str(icono)+' procesado')
            QtGui.QApplication.processEvents()
            resto= (icono + 1) % int(4)

            
            if resto == 0:
                deltaVertical += 43200
                columnaIcono = 0
                
                
                
                
                
                
#                self.ventanaMemoria.textBrowser.append('Se incrementó la fila de datos')
#                QtGui.QApplication.processEvents()
            
            #Fin de lectura de Iconos Horizontales
        # Fin de lectura de Iconos Verticales        


#        
#        for iconosVerticales in range(0,5,1):
#            for iconosHorizontales in range(0,4,1):
#                pixelInicioIcono= (deltaHorizontal * iconosHorizontales) + (deltaVertical * iconosVerticales)
#                #Lectura de un Icono determinado
#                for fila in range(0,altoPixelBytes,1):
#                    for i in range(pixelInicioIcono, pixelInicioIcono + anchoIconoBytes,3):
#                        
#                        colorR= int(rowsPixel[i])        # Obtengo R
#                        colorG= int(rowsPixel[i + 1])    # Obtengo G
#                        colorB= int(rowsPixel[i + 2])    # Obtengo B
#                        colorR= int(colorR & 0x00f8);
#                        colorG1= int(colorG & 0xE0);
#                        colorG2= int(colorG & 0x1C);
#                        colorB= int(colorB & 0xf8);
#                        ch1= (0xff)&( colorR | (colorG1 >> 5) );
#                        ch2= (0xff)&((colorG2 << 3) | (colorB >> 3) );                
#                        rows.append(ch1)
#                        rows.append(ch2)
##                        total += 2
#                        
#                        #fin de lectura de línea de icono
#                    pixelInicioIcono= pixelInicioIcono + anchoTotalBytes  #paso a la siguiente línea del Icono
#                    
#                iconos.append(rows) # guardo como elemento del array iconos al array que contiene el icono leido 
#                # Fin de lectura del Icono completo (60 pixeles x 60 pixeles) 
#            #Fin de lectura de Iconos Horizontales
#        # Fin de lectura de Iconos Verticales

        total= int(len(rows))
        self.ventanaMemoria.textBrowser.append('Cantidad Total: '+ str(total))
        comando= '47$1#'  # Habilita Carga de Buffer
        self.parent.procesaDatoTx(str(comando))
        self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))
        QtGui.QApplication.processEvents()
        time.sleep(1)  
        elementos=0
        contDatos = 0
        
        for i in range(0,int(len(rows)),1):
            
            if int(contDatos) < 10240:
                reg= int(rows[i])
                dato= int(reg)
                self.parent.conexionTx.escribeDatoPuerto(dato.to_bytes(1, byteorder='big'))
                contDatos += 1
                elementos += 1
#                if contDatos % 1000 == 0:
#                    self.ventanaMemoria.textBrowser.append('Transfiriendo: '+ str(contDatos))
#                    QtGui.QApplication.processEvents()
#                time.sleep(0.01)
                            
            if int(contDatos) == 10240:
                texto2 = ''
                termina= 'OK'
                repetir_bucle = True
                contadorTimeOut=0

                while repetir_bucle:
                    texto = str(self.textoCorregido).split('|')
                    texto2= str(texto[0])
                    
                    if texto2 == termina:
                        self.textoCorregido= ""
                        self.ventanaMemoria.textBrowser.append("Datos Transferidos")
                        repetir_bucle = False
                    else:
                        contadorTimeOut += 1
                        if contadorTimeOut == 100:
                            repetir_bucle = False
                            self.ventanaMemoria.textBrowser.append("Time Out: No se transfirieron los datos") 
                        time.sleep(0.1)                

                contDatos = 0
                comando= '15$'+str(address)+'#'  # Carga Buffer en SD
                self.parent.procesaDatoTx(str(comando))
                self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando)+'' )
                QtGui.QApplication.processEvents()
                address = address + 10240
                texto2 = ''
                termina= 'OK'
                repetir_bucle = True
                contadorTimeOut=0

                while repetir_bucle:
                    texto = str(self.textoCorregido).split('|')
                    texto2= str(texto[0])
                    
                    if texto2 == termina:
                        self.textoCorregido= ""
                        self.ventanaMemoria.textBrowser.append("Bloque Guardado - Elemento Actual: "+str(elementos))
                        repetir_bucle = False
                    else:
                        contadorTimeOut += 1
                        if contadorTimeOut == 100:
                            repetir_bucle = False
                            self.ventanaMemoria.textBrowser.append("Time Out: No se guardó bloque") 
                        time.sleep(0.1)
                        
                QtGui.QApplication.processEvents()
                

                if elementos < 144000:
                    time.sleep(0.8)
                    self.ventanaMemoria.textBrowser.append('Elementos: '+str(elementos)+' de '+str(total))
#                    comando= '50$'+str(address)+'#'  # Seteo Address con valor de 32 bits
#                    self.parent.procesaDatoTx(str(comando))
#                    self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))
                    comando= '47$1#'  # Habilita carga de Buffer en SD
                    self.parent.procesaDatoTx(str(comando))
                    self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))
                    
                QtGui.QApplication.processEvents()  #Obligo a realizar la orden gráfica y continuar
                time.sleep(1)  

        for i in range(0,9600,1):
            dato= int(0)
            self.parent.conexionTx.escribeDatoPuerto(dato.to_bytes(1, byteorder='big'))
        comando= '15$'+str(address)+'#'  # Carga Buffer en SD
        self.parent.procesaDatoTx(str(comando))
        self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando)+'' )

        self.ventanaMemoria.textBrowser.append('Imagen: '+str(nombreArchivo)+' transferida. Dirección de origen: '+ str(bloque)+' Total: '+str(elementos)+' bytes de '+str(total)+' bytes')

        
        
        
        
        
#        
#        
#        
#        # Recorro Matriz de Iconos linealizada (2 bytes por pixel)
#        for i in range(0,20,1):
#            for j in range(0,7200,1):
##                self.ventanaMemoria.textBrowser.append('Icono['+str(i)+']['+str(j)+']:'+ str(iconos[i][j]))
#
#                if int(contDatos) < 10240:
#                    dato= int(iconos[i][j])
#                    self.parent.conexionTx.escribeDatoPuerto(dato.to_bytes(1, byteorder='big'))
##                    self.ventanaMemoria.textBrowser.append('Bloque: '+str(valor)+' Dato['+str(contDatos)+']: '+ str(dato))
#                    contDatos += 1
#                    elementos += 1
#                            
#                if int(contDatos) == 10240:
#                    contDatos = 0
#                    comando= '15$'+str(address)+'#'  # Carga Buffer en SD
#                    self.parent.procesaDatoTx(str(comando))
#                    self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando)+'' )
#                    address = address + 10240
#                    texto2 = ''
#                    termina= 'OK'
#                    repetir_bucle = True
#                    contadorTimeOut=0
#    
#                    while repetir_bucle:
#                        texto = str(self.textoCorregido).split('|')
#                        texto2= str(texto[0])
#                        
#                        if texto2 == termina:
#                            self.textoCorregido= ""
#                            self.ventanaMemoria.textBrowser.append("Bloque Guardado - Elemento Actual: "+str(elementos))
#                            repetir_bucle = False
#                        else:
#                            contadorTimeOut += 1
#                            if contadorTimeOut == 100:
#                                repetir_bucle = False
#                                self.ventanaMemoria.textBrowser.append("Time Out: No se guardó bloque") 
#                            time.sleep(0.2)
#        
#                    if elementos < (total):
#                        self.ventanaMemoria.textBrowser.append('Elementos: '+str(elementos)+' de '+str(total))
#        #                    comando= '50$'+str(address)+'#'  # Seteo Address con valor de 32 bits
#        #                    self.parent.procesaDatoTx(str(comando))
#        #                    self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))
#                        comando= '47$1#'  # Habilita carga de Buffer en SD
#                        self.parent.procesaDatoTx(str(comando))
#                        self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))
#    
#                QtGui.QApplication.processEvents()  #Obligo a realizar la orden gráfica y continuar
#            
#            # Restan 9600 bytes que no se utilizan, Completo con CEROS
#        for i in range(0,9600,1):
#            elementos += 1
#            dato= 0
#            self.parent.conexionTx.escribeDatoPuerto(dato.to_bytes(1, byteorder='big'))
#        
#            
#        comando= '15$'+str(address)+'#'  # Carga Buffer en SD
#        self.parent.procesaDatoTx(str(comando))
#        self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando)+'' )
#        self.ventanaMemoria.textBrowser.append('Elementos: '+str(elementos)+' de '+str(total))
#            
#        self.ventanaMemoria.textBrowser.append('Imagen de tipo ICONOS: '+str(nombreArchivo)+' transferida. Dirección de origen: '+ str(bloque))

        



    def transferirBloque(self):
        import time
        bloque= 0
        bloque= self.ventanaMemoria.comboBox_3.currentText()
        address= int(bloque)

        contDatos=0
        rows = []
        rowsPixel= []
        nombreArchivo= self.ventanaMemoria.comboBox.currentText()
        pathImg= "IcECIAA/aplicativos/memoriaSD/imagenes/"+str(nombreArchivo)
        with open(pathImg, "rb") as fp:
            dato = fp.read(1)
            while dato != b"":
                rowsPixel.append(ord(dato))
                dato = fp.read(1)
                

        for i in range(0,int(len(rowsPixel)),3):
            colorR= int(rowsPixel[i])
            colorG= int(rowsPixel[i + 1])
            colorB= int(rowsPixel[i + 2])
            colorR= int(colorR & 0x00f8);
            colorG1= int(colorG & 0xE0);
            colorG2= int(colorG & 0x1C);
            colorB= int(colorB & 0xf8);
            ch1= (0xff)&( colorR | (colorG1 >> 5) );
            ch2= (0xff)&((colorG2 << 3) | (colorB >> 3) );                
            rows.append(ch1)
            rows.append(ch2)
#
#        comando= '50$'+str(address)+'#'  # Seteo Address con valor de 32 bits
#        self.parent.procesaDatoTx(str(comando))
#        self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))
        comando= '47$1#'  # Habilita carga de Buffer en SD
        self.parent.procesaDatoTx(str(comando))
        self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))
        QtGui.QApplication.processEvents()
        time.sleep(1)  
        total= int(len(rows))
        elementos=0
        for i in range(0,int(len(rows))):
            
            if int(contDatos) < 10240:
                reg= int(rows[i])
                dato= int(reg)
                self.parent.conexionTx.escribeDatoPuerto(dato.to_bytes(1, byteorder='big'))
                contDatos += 1
                elementos += 1
                            
            if int(contDatos) == 10240:
                contDatos = 0
                texto2 = ''
                termina= 'OK'
                repetir_bucle = True
                contadorTimeOut=0
                
                while repetir_bucle:
                    texto = str(self.textoCorregido).split('|')
                    texto2= str(texto[0])
                    
                    if texto2 == termina:
                        self.textoCorregido= ""
                        self.ventanaMemoria.textBrowser.append("Datos Transferidos")
                        repetir_bucle = False
                    else:
                        contadorTimeOut += 1
                        if contadorTimeOut == 100:
                            repetir_bucle = False
                            self.ventanaMemoria.textBrowser.append("Time Out: No se transfirieron los datos") 
                        time.sleep(0.1)                    
                
                comando= '15$'+str(address)+'#'  # Carga Buffer en SD
                self.parent.procesaDatoTx(str(comando))
                self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando)+'' )
                address = address + 10240
                texto2 = ''
                termina= 'OK'
                repetir_bucle = True
                contadorTimeOut=0

                while repetir_bucle:
                    texto = str(self.textoCorregido).split('|')
                    texto2= str(texto[0])
                    
                    if texto2 == termina:
                        self.textoCorregido= ""
                        self.ventanaMemoria.textBrowser.append("Bloque Guardado - Elemento Actual: "+str(elementos))
                        repetir_bucle = False
                    else:
                        contadorTimeOut += 1
                        if contadorTimeOut == 100:
                            repetir_bucle = False
                            self.ventanaMemoria.textBrowser.append("Time Out: No se guardó bloque") 
                        time.sleep(0.1)
                        
                QtGui.QApplication.processEvents()
                

                if elementos < (total):
                    time.sleep(0.8)
                    self.ventanaMemoria.textBrowser.append('Elementos: '+str(elementos)+' de '+str(total))
#                    comando= '50$'+str(address)+'#'  # Seteo Address con valor de 32 bits
#                    self.parent.procesaDatoTx(str(comando))
#                    self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))
                    comando= '47$1#'  # Habilita carga de Buffer en SD
                    self.parent.procesaDatoTx(str(comando))
                    self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))
                    QtGui.QApplication.processEvents()
                    time.sleep(1)  

                QtGui.QApplication.processEvents()  #Obligo a realizar la orden gráfica y continuar

        self.ventanaMemoria.textBrowser.append('Imagen: '+str(nombreArchivo)+' transferida. Dirección de origen: '+ str(bloque)+' Total: '+str(elementos)+' bytes de '+str(total)+' bytes')



    def transferirFuente(self):
        import time
        bloque=self.ventanaMemoria.lineEdit.text()
        valor= int(bloque)
        comando= '50$'+str(valor)+'#'  # Seteo Address con valor de 32 bits
        self.parent.procesaDatoTx(str(comando))
        self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))
        
        comando= '47$1#'  # Habilita Carga de Buffer
        self.parent.procesaDatoTx(str(comando))
        self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))
        self.ventanaMemoria.textBrowser.append('Leyendo archivo...')
        contDatos=0
        rows = []
        nombreArchivo= self.ventanaMemoria.comboBox_2.currentText()
        pathImg= "IcECIAA/aplicativos/memoriaSD/fuentes/"+str(nombreArchivo)
        with open(pathImg, "r") as fp:
            datos = fp.readlines()
            for linea in datos:
                rows.append(int(linea.rstrip('\n')))                

        for i in range(0,int(len(rows))):
            if int(contDatos) < 512:
                reg= int(rows[i])
                dato= int(reg)
                self.parent.conexionTx.escribeDatoPuerto(dato.to_bytes(1, byteorder='big'))
    #                self.ventanaMemoria.textBrowser.append('Bloque: '+str(valor)+' Dato['+str(contDatos)+']: '+ str(dato))
                contDatos += 1
                            
            if int(contDatos) == 512:
                contDatos = 0
                comando= '46$1#'  # Carga Buffer en SD
                self.parent.procesaDatoTx(str(comando))
#                self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))
                time.sleep(0.1)
                valor = int(valor + 512)   # Address del Proximo bloque
                comando= '50$'+str(valor)+'#'  # Seteo Address con valor de 32 bits
                self.parent.procesaDatoTx(str(comando))
#                self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))
                comando= '47$1#'  # Habilita carga de Buffer en SD
                self.parent.procesaDatoTx(str(comando))
#                self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))
                        
            QtGui.QApplication.processEvents()  #Obligo a realizar la orden gráfica y continuar
        delta = int(512 - int(contDatos))
        if delta > 0:
            for var in range(0,delta):
                dato= 0              
                self.parent.conexionTx.escribeDatoPuerto(dato.to_bytes(1, byteorder='big'))
#                self.ventanaMemoria.textBrowser.append('Bloque: '+str(valor)+' Dato['+str(contDatos)+']: '+ str(dato))
                contDatos += 1
    
        comando= '46$1#'  # Carga Buffer en SD
        self.parent.procesaDatoTx(str(comando))
        self.ventanaMemoria.textBrowser.append('Imagen: '+str(nombreArchivo)+' transferida. Dirección de origen: '+ str(bloque))


    def listaDireccionesImagen(self):
        for elemento in range(0,15360000,153600):
            self.ventanaMemoria.comboBox_3.addItem(str(elemento).strip())
        self.ventanaMemoria.comboBox_3.setCurrentIndex(0)



    def listaIconos(self):
        os.system('ls IcECIAA/aplicativos/memoriaSD/imagenes/iconos/ > iconosSD.lst')
        fp= open('iconosSD.lst', 'r')
        lista = list(fp)
        fp.close
        os.system('rm iconosSD.lst')
        for elemento in lista:
            self.ventanaMemoria.comboBox_4.addItem(str(elemento).strip())
        self.ventanaMemoria.comboBox_4.setCurrentIndex(0)


    def listaImagenes(self):
        os.system('ls IcECIAA/aplicativos/memoriaSD/imagenes/ > imagenesSD.lst')
        fp= open('imagenesSD.lst', 'r')
        lista = list(fp)
        fp.close
        os.system('rm imagenesSD.lst')
        for elemento in lista:
            self.ventanaMemoria.comboBox.addItem(str(elemento).strip())
        self.ventanaMemoria.comboBox.setCurrentIndex(0)


    def listaFuentes(self):
        os.system('ls IcECIAA/aplicativos/memoriaSD/fuentes/ > fuentesSD.lst')
        fp= open('fuentesSD.lst', 'r')
        lista = list(fp)
        fp.close
        os.system('rm fuentesSD.lst')
        for elemento in lista:
            self.ventanaMemoria.comboBox_2.addItem(str(elemento).strip())
        self.ventanaMemoria.comboBox_2.setCurrentIndex(0)
        
        
    def inicializarMemoria(self):
        import time
        comando= '36$1#'
        self.parent.procesaDatoTx(str(comando))
        self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))
        time.sleep(0.2)
        comando= '37$1#'
        self.parent.procesaDatoTx(str(comando))
        self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))
        
    def leerBloquesMemoria(self):
        dirInicial=int(self.ventanaMemoria.lineEdit_4.text())
        dirFinal=int(self.ventanaMemoria.lineEdit_5.text())
        cantBloques= int((dirFinal - dirInicial)/512)
        if cantBloques == 0:
            cantBloques = 1
        for i in range(0,cantBloques):
            valor= dirInicial + int(i*512)
            comando= '39$'+str(valor)+'#'
            self.parent.procesaDatoTx(str(comando))
            self.ventanaMemoria.textBrowser.append('Comando: '+ str(comando))

        
    def activaTerminalMemoria(self):
        self.thTerminalMemoria.daemon=True
        self.thTerminalMemoria.start()    

    def terminalMemoriaSD(self):
        import time
        self.ventanaMemoria.textBrowser.append('Leyendo datos del puerto serie...')
        contador=0
        texto=''
        while True:
            if self.habilitaTerminalMemSD:
                try:
                    texto= self.parent.conexionRx.leePuerto()  # str(texto) +' '+ str(contador) 
                    self.textoCorregido= str(texto).replace("\\n","")
                    self.textoCorregido= str(self.textoCorregido).replace("b'","")
                    self.textoCorregido= str(self.textoCorregido).replace("'","")
                    self.datoRecibido= self.textoCorregido
                    self.ventanaMemoria.textBrowser.append(str(self.textoCorregido))
                    self.ventanaMemoria.textBrowser.verticalScrollBar().setValue(self.ventanaMemoria.textBrowser.verticalScrollBar().maximum())
                    QtGui.QApplication.processEvents()  #Obligo a realizar la orden gráfica y continuar
                except:
                    pass
                contador+= 1
                time.sleep(0.07)
            else:
                time.sleep(0.5)
                self.ventanaMemoria.textBrowser.verticalScrollBar().setValue(self.ventanaMemoria.textBrowser.verticalScrollBar().maximum())
                QtGui.QApplication.processEvents()  #Obligo a realizar la orden gráfica y continuar
                self.ventanaMemoria.textBrowser.append('Proceso Recepción Finalizado')
                
    def toHex(dec):
        x = (dec % 16)
        digits = "0123456789ABCDEF"
        rest = dec / 16
        if (rest == 0):
            return digits[x]
        return toHex(rest) + digits[x]


    def closeEvent(self, event):
        self.parent.habilitaTerminalRx= True
        self.setWindowFlags(Qt.FramelessWindowHint | Qt.Popup)
        