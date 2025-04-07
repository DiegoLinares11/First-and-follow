# First and follow

## Estructura a seguir:

grammar: Guarda las reglas como un mapa. Cada no terminal se asocia con una lista de producciones, donde cada producción es una lista de símbolos.

    Ejemplo:
    VP -> VP PP | V NP | eats
    se guarda como:
    "VP" → [ [ "VP", "PP" ], [ "V", "NP" ], [ "eats" ] ]

nonTerminals: Conjunto de símbolos no terminales (los que aparecen a la izquierda de ->).

terminals: Se infieren automáticamente. Son todos los símbolos que aparecen en las producciones y no son no terminales.

firstSet: Guarda los conjuntos FIRST ya calculados (memoización).

### Algoritmo de First

La función computeFirst(symbol) funciona así:

    Caso base:

        Si symbol es un terminal → FIRST(symbol) = { symbol }

        Si symbol = ε → FIRST(symbol) = { ε }

    Caso recursivo (cuando symbol es un no terminal):

        Para cada producción de symbol, recorre los símbolos de izquierda a derecha.

        Agrega los símbolos de FIRST(X) al conjunto.

        Si todos los símbolos de una producción pueden derivar ε, entonces ε se agrega al resultado.

    Memoización:

        Se guarda FIRST(symbol) ya calculado para evitar recalculaciones.


### Algoritmo de follow

 Reglas generales para calcular FOLLOW(A)

    Regla 1: Si A es el símbolo inicial → FOLLOW(A) contiene $ (fin de cadena).

    Regla 2: Si hay una producción B -> α A β, entonces todo en FIRST(β) (excepto ε) va a FOLLOW(A).

    Regla 3: Si B -> α A o B -> α A β y FIRST(β) contiene ε, entonces FOLLOW(B) va a FOLLOW(A).