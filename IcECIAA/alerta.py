# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'alerta.ui'
#
# Created by: PyQt5 UI code generator 5.6
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_alerta(object):
    def setupUi(self, alerta):
        alerta.setObjectName("alerta")
        alerta.resize(450, 161)
        self.label = QtWidgets.QLabel(alerta)
        self.label.setGeometry(QtCore.QRect(30, 20, 391, 71))
        self.label.setText("")
        self.label.setObjectName("label")
        self.pushButton = QtWidgets.QPushButton(alerta)
        self.pushButton.setGeometry(QtCore.QRect(110, 120, 90, 28))
        self.pushButton.setObjectName("pushButton")
        self.pushButton_2 = QtWidgets.QPushButton(alerta)
        self.pushButton_2.setGeometry(QtCore.QRect(240, 120, 90, 28))
        self.pushButton_2.setObjectName("pushButton_2")

        self.retranslateUi(alerta)
        QtCore.QMetaObject.connectSlotsByName(alerta)

    def retranslateUi(self, alerta):
        _translate = QtCore.QCoreApplication.translate
        alerta.setWindowTitle(_translate("alerta", "Confirmación"))
        self.pushButton.setText(_translate("alerta", "Aceptar"))
        self.pushButton_2.setText(_translate("alerta", "Cancelar"))

