# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ayuda.ui'
#
# Created by: PyQt5 UI code generator 5.6
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_ayuda(object):
    def setupUi(self, ayuda):
        ayuda.setObjectName("ayuda")
        ayuda.resize(871, 597)
        self.textBrowser = QtWidgets.QTextBrowser(ayuda)
        self.textBrowser.setGeometry(QtCore.QRect(0, 70, 871, 471))
        self.textBrowser.setObjectName("textBrowser")
        self.label = QtWidgets.QLabel(ayuda)
        self.label.setGeometry(QtCore.QRect(410, 20, 141, 51))
        font = QtGui.QFont()
        font.setPointSize(20)
        font.setBold(True)
        font.setItalic(True)
        font.setWeight(75)
        self.label.setFont(font)
        self.label.setObjectName("label")
        self.label_2 = QtWidgets.QLabel(ayuda)
        self.label_2.setGeometry(QtCore.QRect(330, 10, 71, 61))
        font = QtGui.QFont()
        font.setPointSize(20)
        font.setBold(True)
        font.setItalic(True)
        font.setWeight(75)
        self.label_2.setFont(font)
        self.label_2.setText("")
        self.label_2.setPixmap(QtGui.QPixmap("imagenes/ice.png"))
        self.label_2.setScaledContents(True)
        self.label_2.setObjectName("label_2")
        self.pushButton = QtWidgets.QPushButton(ayuda)
        self.pushButton.setGeometry(QtCore.QRect(390, 560, 90, 28))
        self.pushButton.setObjectName("pushButton")

        self.retranslateUi(ayuda)
        QtCore.QMetaObject.connectSlotsByName(ayuda)

    def retranslateUi(self, ayuda):
        _translate = QtCore.QCoreApplication.translate
        ayuda.setWindowTitle(_translate("ayuda", "Ayuda"))
        self.label.setText(_translate("ayuda", "IceCIAA"))
        self.pushButton.setText(_translate("ayuda", "Cerrar"))

