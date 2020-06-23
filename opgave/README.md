---
title: To B or not to B-tree
subtitle: Project Algoritmen en Datastructuren 2019-2020
papersize: A4
pagestyle: headings
colorlinks: true
numbersections: true
lang: nl-BE
geometry: a4paper
---

# Inleiding

Er is je gevraagd om een databank te maken voor een programma dat verschillende tijdslijnen simuleert en evenementen registreert. De databank moet heel snel het aantal evenementen binnen een bepaalde tijdspanne kunnen bepalen. Deze waarden zijn immers nodig om de simulatie verder uit te voeren.

Om dit te doen, maak je gebruik van een aangepaste B-tree die snel intervallen van data kan samenvatten zonder alle toppen te moeten overlopen. Om een zo hoog mogelijke snelheid te verkrijgen onderzoek je hoe je de toppen van de boom best opslaat om betere caching eigenschappen te hebben.

# Functionele vereisten

Schrijf een programma `history` dat evenementen bijhoudt in een **B-tree**. Je programma kan via de standaard invoer volgende commando's krijgen:

- `+`:  sla een evenement op bij een bepaald tijdstip
- `-`: verwijder een evenement
- `?`: geef de gegevens van het evenement op een gegeven tijdstip
- `#`: bepaal het aantal evenementen tussen twee tijdstippen (inclusief eindpunten)

Zorg ervoor dat je programma een goede caching strategie heeft.

## Voorstelling tijdstippen

Tijdstippen worden weergegeven als strings in [ISO 8601](https://nl.wikipedia.org/wiki/ISO_8601) notatie zonder tijdzone (de deadline van dit project wordt bijvoorbeeld genoteerd als: "`2019-12-02T13:37:42`"). Voor de eenvoud kun je gebruik maken van de lexicografische orde die ISO 8601 je geeft om tijdstippen te vergelijken. Het is niet nodig om de binnenkomende tijdstippen op eender welke manier te parsen. Je hoeft ook geen rekening te houden met tijdzones en dergelijke. Tijdstippen **moet** je voor dit project zien als gewone strings van 19 karakters.

## Commando's

Je programma moet de `+`, `-`, `?` en `#` commando's aanvaarden en daar op een juiste manier mee omgaan. Het is toegestaan om zelf extra commando's toe te voegen aan je programma zo lang die niet interfereren met de gevraagde commando's. 

### Toevoegcommando (`+`)

```
"+" DATUM " " INHOUD "\n"
```

Het toevoegcommando is een reeks met achtereenvolgens

- een `+` teken,
- een ISO 8601 representatie van een tijdstip (zonder tijdzone),
- een spatie "` `",
- de inhoud van het evenement als extended ASCII string,
- een afsluitende nieuwe lijn (`\n`)

Als een toevoegcommando wordt ingelezen, moet `INHOUD` in de databank geplaatst worden onder de sleutel `DATUM`. Als er al een waarde in de databank zit voor die `DATUM`, moet deze overschreven worden. Het toevoegcommando genereert geen output.

Voorbeelden:

```
+2019-01-01T12:10:37 Inval van Gargamel in het Smurfendorp
+2019-01-01T18:04:12 Smurfin bevalt van haar eerste zoon
+2019-04-01T12:15:14 Mopsmurf veroorzaakt het knoflookpoederfiasco
```

### Verwijdercommando (`-`)

```
"-" DATUM "\n"
```

Het verwijdercommando is een reeks met achtereenvolgens

- een `-` teken,
- een ISO 8601 representatie van een tijdstip (zonder tijdzone),
- een afsluitende nieuwe lijn (`\n`)

Bij het verwerken van een verwijdercommando verwijder je het evenement dat op het tijdstip `DATUM` in de databank zit. Bij een succesvolle verwijdering schrijf je `-\n` naar de standaard uitvoer. Bij een gefaalde verwijdering, bijvoorbeeld als het tijdstip niet in de databank voorkomt, schrijf je `?\n` uit. Gezien er maar 1 evenement is per tijdstip, is een opzoeking naar het tijdstip dat net verwijderd is altijd onsuccesvol.

Voorbeeld:

```
-2019-04-01T12:15:14
```

Output

```
-
```



### Opvraagcommando (`?`)

```
"?" DATUM
```

Een opvraagcommando is een reeks met achtereenvolgens

- een `?` teken,
- een ISO 8601 representatie van een tijdstip (zonder tijdzone),
- een afsluitende nieuwe lijn (`\n`)

Na het inlezen van een opvraagcommando, schrijf je naar standaard uitvoer 

- het teken `!` onmiddellijk gevolgd door de extended ASCII string die het evenement op dat tijdstip beschrijft  als er een evenement is op dat exacte tijdstip.
- het teken `?` als er geen evenement is op het gegeven tijdstip

gevolgd door een nieuwe lijn (`\n`) 

Voorbeeld (lijnen afwisselend input en output)

```
?2019-01-01T12:10:37
!Inval van Gargamel in het Smurfendorp
?2019-01-01T23:10:37
?
```



### Telcommando (`#`)

```
"#" DATUM " " DATUM
```

Een telcommando is een reeks met achtereenvolgens

- een `#` teken,
- een ISO 8601 representatie van een begintijdstip (zonder tijdzone),
- een spatie
- een ISO 8601 representatie van een eindtijdstip (zonder tijdzone),
- en een afsluitende nieuwe lijn (`\n`)

Na het inlezen van een telcommando schrijf je naar standaard uitvoer het aantal evenementen tussen het begintijdstip en eindtijdstip (inclusief eindpunten, ook omgekeerd). Een evenement is tussen twee tijdstippen als zijn [ISO 8601](https://nl.wikipedia.org/wiki/ISO_8601) tijdsnotatie lexicografisch tussen de [ISO 8601](https://nl.wikipedia.org/wiki/ISO_8601) tijdsnotaties van die tijdstippen is. Na het uitschrijven van dit getal schrijf je een nieuwe lijn (`\n`). *Merk dus op dat een of beide tijdstippen mogelijk niet in de databank kunnen zitten*. 

Voorbeeld (lijnen afwisselend input en output)

```
#2019-01-01T23:10:37 2020-01-01T23:10:37 
4
#2019-01-01T23:10:37 2018-01-01T23:10:37 
0
```