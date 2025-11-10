# 🗳️ Sistema de Gestión del Proceso Electoral Presidencial Chileno

Este proyecto es un sistema de software robusto y eficiente diseñado para soportar y gestionar integralmente las diversas etapas del proceso electoral presidencial en Chile. La plataforma cubre desde el registro de candidatos y votantes hasta la proclamación oficial de resultados, con un enfoque en la precisión, transparencia y seguridad.

## Características Principales

El sistema ofrece una solución completa para la administración electoral, incluyendo:

* **Registro de Candidaturas:**
    * Gestión de candidatos de partidos políticos e independientes.
    * Validación de requisitos legales (edad, nacionalidad).
    * Mecanismo para verificar el apoyo popular de independientes (ej. recolección de firmas).

* **Gestión del Padrón Electoral:**
    * Inscripción y actualización de votantes, tanto en territorio nacional como en el extranjero.
    * Plataforma de consulta y actualización de datos personales (dirección, local de votación).

* **Votación y Mesas:**
    * Soporte para mesas de votación (posiblemente electrónicas) que garantizan el voto secreto.
    * Sistema para facilitar el conteo de votos.
    * Control y registro de la abstención (quienes no votaron).

* **Escrutinio y Cómputo:**
    * Módulo de escrutinio automático para el conteo de votos válidos, nulos y en blanco.
    * Generación de resultados preliminares en tiempo real de forma segura y precisa.

* **Declaración de Resultados:**
    * Módulo para la proclamación oficial y automática del candidato ganador por parte de las autoridades.
    * Generación de informes detallados sobre participación y porcentajes de votos.

* **Administración de Datos (CRUD):**
    * Un **menú de usuario interactivo** en consola que permite la gestión completa de todas las estructuras de datos (candidatos, votantes, mesas, etc.).
    * Funcionalidades completas: **Agregar, Eliminar, Buscar, Modificar y Listar** todos los registros.

* **Funciones Adicionales:**
    * El sistema incluye módulos extra para análisis y reportes, como cálculos estadísticos de participación, seguimiento de la abstención, etc.

## Arquitectura y Diseño Técnico

El núcleo del sistema está construido sobre una arquitectura de datos robusta para manejar la complejidad de la información electoral:

* **Estructuras de Datos Anidadas:** El software combina de forma anidada:
    * Estructuras estáticas.
    * Estructuras dinámicas simples (ej. Listas enlazadas).
    * Estructuras dinámicas complejas (ej. Árboles Binarios de Búsqueda - ABB).

* **Encapsulamiento:** Se aplica el encapsulamiento para el manejo de las estructuras de datos dinámicas, promoviendo un código modular, mantenible y seguro.

* **Complejidad de Datos:** Las estructuras de datos principales no son de tipo primitivo; contienen otras estructuras anidadas (compuestas por al menos 3 tipos primitivos) para representar de forma fiel la complejidad de las entidades (ej. un "Votante" o "Candidato").

* **Algoritmos:**
    * Integra algoritmos de **búsqueda** eficientes para localizar rápidamente información en el sistema.
    * Implementa algoritmos de **ordenamiento** para presentar los datos de manera organizada.

## Tecnologías

* **Lenguaje:** ANSI C
* **IDE Recomendado:** JetBrains CLion
* **Principios:** El código fuente está modularizado, documentado y sigue buenas prácticas de legibilidad.

## Uso

1.  Clonar el repositorio.
2.  Abrir la carpeta del proyecto con JetBrains CLion (o compilar manualmente los archivos fuente `*.c`).
3.  Compilar y ejecutar el programa principal.
4.  Navegar por el menú interactivo que se presentará en la consola.
