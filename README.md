# GRK_Projekt
### Autorzy: Katarzyna Zbroja(426289) i Kacper Kruczek(426159)

## Przedmiotem poniższej dokumentacji jest projekt akwarium spełniający poniższe wymagania:


1.  Scena pokazuje ryby, pływające wokół starożytnych ruin.
2.  Można wejść w interakcję ze sceną i scena jest animowana
3.  Wszystkie wielokąty są teksturowane
4.  Wszystkie wielokąty są oświetlone.

---

<dl>
<dt>Założeniem projektu jest ławica ryb, pływająca wokół starożytnych ruin.  </dt>
<dd>Na tło wykorzystano obiekt kuli, z nałożoną proceduralną teksturą, generowaną za pomocą Fractal Brownian Motion, który jest przyczepiony do pozycji kamery i </dd><dd>razem z nią się porusza.</dd>
</dl>
<dl>
  
---

<dt>Kamerą można poruszać:<dt>
<dd>
* "w"-do przodu widoku;
* "s" - do tyłu widoku,
*"a" - w prawo widoku,
* "d" - w lewo widoku,
* "r" - w gore widoku,
* "f" - w dol widoku,
* "q" - obrót w lewo,
* "e" - obrot w prawo,
* "t" - tryb "turbo"(przyszpieszenie poruszania),
* "p" - zatrzymaniue ruchu rybek,
* "l" - wyświetlenie w konsoli lokacji kamery
<dd>
</dl>

---

Każda rybka porusza się po krzywej, z różnymi prędkościami, oraz różnym odstępem od niej.
Każda rybka ma nałożoną na sobą proceduralną teksturę, rysującą na niej paski.
Scenę można zatrzymać w każdym momencie, po powrocie do ruchu, rybki poruszają się znowu, od kolejnej pozycji.
<img src ="https://github.com/neronim1141/GRK_Projekt/blob/master/Pokaz.gif" width="100%">
