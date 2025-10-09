# Pa55w0rd Avalanche
## Generate safe passwords from easy-to-remember words or sentences

- Designed and written by Guillermo Amodeo Ojeda.

```Pa55w0rd Avalanche``` is an algorithm I invented to convert a small word or sentence to a huge string of random characters, in order to be able to use passwords that you can easily remember instead of using random chars strings, which you have to write down and hide somewhere. To me this is the problem with password these days: **Rules make passwords too complex to remember**.

My algorithm works by generating an avalanche of bits from hashing 64 bytes of random data generated using a pseudo-random generator, chosen by the first four bytes of the key, and then using these same four bytes to seed the generator, this 64 bytes are then treated as the first temporary hash, which the algorithm repeatedly hash together with the original word and converts the result to chars in the 'avalanche alphabet'. It then repeats this process until enough chars are obtained, reusing the 64 bytes of the resulting hash as the temporary hash for the next step of the process.

Therefore, if you feed a word like ```test``` to this algorithm, you would obtain the following passwords depending on the output size, which is shown beside them:

- 12 ```p=t32_C!)HI!#```
- 20 ```P3#b^w$kGRa7VPoox@tPZ```
- 35 ```No8R=#HQHr@DoM^!~2cx9[$Td|_vwoxd{N4A```
- 55 ```VgkoxBH)d12}r@J}M#Y/tREXU+Iu5t!B#G{!6oP(QtNIeeG2f(&jeDb@```

The generated passwords not only fit all the chars type requirements, they can be as long as you want to increase the difficulty of a brute-force attack.

As the algorithm is deterministic -the same word/sentence always generates the same long password-, an attacker could try a brute-force attack on your long password using the algorithm to generate them, for example trying by avalanching all the words of a dictionary. The thing is that they cannot be sure of which is the correct password so easily, as there is an extra variable in there: you can choose the length of the output, which will output a different password from he same word, so the attacker also needs to find out what is the size you used in order to find out which of the generated passwords is the correct.

Let me rephrase the previous statement: ```The algorithm is deterministic, so the same word/sentence will generate the same long password, only for the same ouput size. If you change the size, the password will also change```.

The problem I solve with this algorithm is that these days the amount of requirements for passwords have increased hugely, and also you need two passwords everywhere. Yeah, the so called 'Multi Factor Authentication' is nothing else than 2 or more passwords, being one of them an OTP number, being OTP the anagram of 'One Time Password'. Oh yeah, that number is a second password to protect the first password (in case it has been compromised).

On top of that, you are required that your first password is *safe*, which for most people define as: sort of 8-12 chars long (12 better than 8) and at least 4 of these chars are 1 lower case, 
1 upper case, 1 number and 1 symbol.

Then, we have to add this other rule that say: *you should not be using a password like ```Pa55w0rd!``` because it is too common*, so even if it fits the requirements: 9 chars, of which 4 fit the asked types, you need to find something *better*, better being something like ```$;aR{[eX3```, which I don't see better -as computationally speaking- its actually easier to find than ```Pa55w0rd!``` using brute force. Yeah, the symbols have lower char values than letters and numbers so they will be tested before and found faster.

Now, the reason why ```$;aR{[eX3``` is considered better than ```Pa55w0rd!``` probably is for protecting your password against a dictionary attack, where a dictionary composed of known words and variations with the char type requirements is offuscated with the same technique than the attacked system, and then compared to find matches.

To perform this attack succesfully, an attacker needs two things:

- Access to the password database
- Knowledge of the technique used to cipher the passwords

This make this attack difficult to perform for most people, but not impossible for everyone, as hackers **do** get hold of password files, and *anyone* can buy them in the Dark Web, which is why administrators moved password hashing from salts to stuff similar to bcrypt(), which offers some protection against the second thing above, as knowing is using bcrypt() doens't help to crack it.

This is why ```Pa55w0rd Avalanche``` let you generate different variable size passwords from the same word, so the attacker needs to try many sizes which increases the number of combinations needed to perform a succesful brute-force attack, even using *rainbow tables*.

You can use this algorithm in two ways:

- To create an utility to generate a pasword from a given word or sentence -like the test program does-, which the user can copy & paste to the correct app.
- Embbed the algorithm into your system so the user can just type a word or sentence, which the system uses to generate the safe password and then use it as the actual password.

In order to make these two protection options useful, the length of the generated password has to be chosen by the user and not the system, as it will be giving away the lenght for the generated password, making the life of attackers easier.

Do not forget to advise your users that if they copy/paste a password, they should inmediately copy few things in a row to the clipboard so these stuff will overwrite the password and will not be compromised as any program can grab the clipboard, and therefore is the first place the attackers look when stealing passwords. 

You can also provide your system with means to copy the password to the clipboard, and also to *safely* clear the clipboard after pasting it to the other app.

## To build it:

For Microsoft C/C++ for Windows (Visual Studio C++) use:  

```
nmake /f msc.mak test
```

For Macintosh, Linux and others using GNU C:

```
make -f gcc.mak test
```
### To clean the builds:

Do the same that for build it but use *cleanit* instead of *test* as the target.

## Example output from the test program:

```

$ ./test_avalanche ?

**** Test of pa55w0rd_avalanche() ****

usage: ./test_avalanche  [num_chars] [[word or "quoted sentence to avalanche"] ... ]

Where num_chars is the number of chars generated by the function (8 - 250)

$ ./test_avalanche 20 popeye
20 - 5t@@OaS&L(cEKhP-Nlem

$ ./test_avalanche 61 popeye
61 - /!=F6T-XeslcRpLg(H1MZK4@]^8$bF@dp##-(eGx]Q^mQ+3oJXhb~g8xYO~II

$ ./test_avalanche

**** Test of pa55w0rd_avalanche() ****

pa55w0rd_avalanche(             ) = 35 - Sw9]8#lUbQZFBtRAQ+gC^!~Z9xn7^[W#PEU
pa55w0rd_avalanche(            a) = 35 - iLd^yo{bQx_Y#@H^fDtn395AFWFT6lt2Nn8
pa55w0rd_avalanche(   atmosphere) = 35 - H]6!UxN/G|KeJskQu!~IPee8~4UxTH$$5#]
pa55w0rd_avalanche(   basketball) = 35 - 2UQL=5aA!s-MRkh+ZbE#zb3T8llbw=@c_/Y
pa55w0rd_avalanche(         bury) = 35 - 5ho_T1tH3e^3crNW_M&zr^n@&@l][N8|xmF
pa55w0rd_avalanche(concentration) = 35 - LU@x]3l@=7vh=!szYwgzl7Q/^[xx#H&@/9-
pa55w0rd_avalanche(         copy) = 35 - !QdGhW{@n!HZG9MD1@9Al!$k|5Vl!wx#5ga
pa55w0rd_avalanche(          dog) = 35 - W_eLZwy&A6d~ga#L/bI42[^dx0$M!j^h0BC
pa55w0rd_avalanche( dramatically) = 35 - t^&3#@^[9xW2HpiDMYL]3jDB1NrcMzx(##!
pa55w0rd_avalanche(establishment) = 35 - 9@e0K#@7LZ]z2H-j$Zx0ctM3kQDww+X(__B
pa55w0rd_avalanche(         ever) = 35 - #N@65(azkK!xF/j62ii~3])2zBS$9plae-G
pa55w0rd_avalanche(  frustration) = 35 - N|-x(!=Ud7@x~1|kRS^-^_O#8!QRgnSVPod
pa55w0rd_avalanche(         fund) = 35 - RW8h8~lt0Sf-Pf$Ero$$z]@J!Y{UP6Xiyhr
pa55w0rd_avalanche(  grandfather) = 35 - -^K/(S$V!L4}!MdY30Q3UFH^XsLOociSSrb
pa55w0rd_avalanche(           go) = 35 - KiFSQa]lUBBn&i@dl@6V#Yjm#4)IW0he4wZ
pa55w0rd_avalanche(           hi) = 35 - @BDKydioG7#W}zIYF8B=$dhhpo!P|LeIfi7
pa55w0rd_avalanche(   hypothesis) = 35 - Su#IV5d!R^jXlva/_!~@_j}wNT7$#gx35!]
pa55w0rd_avalanche(            I) = 35 - 37EvA#lzS33pF@ejRF3tKEb~BY/X0(jkdg=
pa55w0rd_avalanche(  involvement) = 35 - JtJL!9JS(r9LL!m}WVBi6IL)1h9JnyO!xPs
pa55w0rd_avalanche(         joke) = 35 - u!FWJAdpK+W#h0k5V5&0ZJf8!3Q{dp@3hxw
pa55w0rd_avalanche(   journalist) = 35 - xDi#lSm@+YC8|)/bu!JH4U)]w9{P~J!$3!4
pa55w0rd_avalanche(         keep) = 35 - gv8!!3oQBf+h2jllfWG+cWYYW-4fyOj[@b9
pa55w0rd_avalanche(     kangaroo) = 35 - rJpP$69zZUkFO4#unDDagR41xCj+v/QcZ2Z
pa55w0rd_avalanche(   literature) = 35 - @hvffz!^^=RODF&JdtTxdW8n!#L85tBMn/H
pa55w0rd_avalanche(          low) = 35 - M5+j7[2!{b~EH@Mt{ZWa$3L+hFuj4_b${QO
pa55w0rd_avalanche(         moon) = 35 - __x}WCxnh-|#M([!Lw-!]QU}D0{Eb3I!a7J
pa55w0rd_avalanche(   motivation) = 35 - r)@vN[&u{z74!kLfg=m(2h]]ym2QOctNPX@
pa55w0rd_avalanche(         near) = 35 - w@B!#NPH)418{/xcLHcanUa/xn[Jx8)Bc+=
pa55w0rd_avalanche(  negotiation) = 35 - 1gzBZmwEQl4_!xMy#Yhz=!(^0/o-Zo-edAi
pa55w0rd_avalanche(    otherwise) = 35 - @cuP/!$Wx2$#KUT@Y^vk^o]Ro#/sF^=711N
pa55w0rd_avalanche(          own) = 35 - JZ]76n(aXjbT7)u6)HwCn#EPiltabl7sxn@
pa55w0rd_avalanche(   punishment) = 35 - p^QQNCHG#6@@~~D84!fc0~Rx[m^9$d2x[=1
pa55w0rd_avalanche(          put) = 35 - bB()GGG5@pY@^2c{uJzp=i6!xWsL@xmO{VH
pa55w0rd_avalanche(  quarterback) = 35 - 5/u/e0}k{#y/)N82AuwwypbyN6vJZ[bk+5O
pa55w0rd_avalanche(         quit) = 35 - })VBLK1&S4@_@G0c|DiTRYuQ!gb)jZL@RbL
pa55w0rd_avalanche(  responsible) = 35 - (-SRxEUL+MXSZZgxnMo{XvH!sw}|Em4ZL_7
pa55w0rd_avalanche(         rise) = 35 - $w}[Z/GymcUZwW$|m$FRAs/dgo|C9YIDsFE
pa55w0rd_avalanche( specifically) = 35 - sJ6G!0cU0)@tE{{&1@KHOfj+x{WF1r!Yw!!
pa55w0rd_avalanche(         spot) = 35 - ch!R=!=82^(!7^+iT!24R5vML4i7^w!2R=A
pa55w0rd_avalanche(   tremendous) = 35 - 2aoO@)GBZjNkJ@!!i[=o4IFm@]XGLrTL=7K
pa55w0rd_avalanche(        trust) = 35 - @Yu{$LdW#}pU~}~nN&ygZE)VVki8!7GrN8#
pa55w0rd_avalanche(         ugly) = 35 - 5!=4}og~J(!LxFTNDlz82r&Xt)A#pFi@+R)
pa55w0rd_avalanche(      utility) = 35 - $7XB|{#@Vf5]^LHG7bVl(YrI!5!|&_k7Kr^
pa55w0rd_avalanche(         vast) = 35 - pbH~@f$b(y_|{@e#]_@85h(n&uX}VO^9!o/
pa55w0rd_avalanche(      visible) = 35 - 6w+m=Hcwt@C[S@!UPUPY@Ei3g}+&pfMB254
pa55w0rd_avalanche(     whatever) = 35 - Ex/w^c[!!fP[FRU^6(X0Q|$GBluY#EU!YKM
pa55w0rd_avalanche(       window) = 35 - PD0IR{8@w}UB@PP|Hkere{8~))rJE#U|k!N
pa55w0rd_avalanche(         yard) = 35 - j4$x0w1kH^^&Qrpt#J=QHTx@D=9onbV5wNQ
pa55w0rd_avalanche(     yourself) = 35 - +@22b-((5t5&i5=}e3kPF6LE3#@(e5UJCK!
pa55w0rd_avalanche(         zulu) = 35 - sKb#HYYOe@P]Tp_/A~(&G^ALkX8!F+T!VQ[
pa55w0rd_avalanche(      zachary) = 35 - 0agc9w)^H]+a[K@z${vM=)NaGe@Hl3@L6_m

$ ./test_avalanche 165

**** Test of pa55w0rd_avalanche() ****

pa55w0rd_avalanche(             ) = 165 - TV[SAwOe#ws/pnb7|Jnaa}M&a#4IK]KV@@&Je}jiYKx)|az#sNw~cD1fDvAcPZd6LmC=YzwMNm2EdbEj#$iW@v/poko/e@(Mj=^~o9KE1&@M@ITLj^hlZHfIuSw#r_@0!^[Vk7sXiiaQCorW7hGCge8[2dLjwdNJH^=MC
pa55w0rd_avalanche(            a) = 165 - iixohNg^@TR^r!wbtxdK$iOb^B0M!d0^VM8Tc)3PGzFlU@mcSO@8ZjIRo9b|}r=po5b(7nX=^(}jSi3$]w~B]R#)YtgL!HP+m]--fX&g-H!X+MCGp#)x@)i!H|YuLVN02!}!P8fx$zxCXzA]#c|wXf3{mkesYZ/XVF#re
pa55w0rd_avalanche(   atmosphere) = 165 - @+lMIPu#QDMwK$)J3]3oK$@@+@8xD|V^QC!O|lp!jhl!h~b/Hn!K^5(!de0S{XF@2vHnnC4n4k{pmfWE[wOzrvahH^|Y_PFA#W!y6+GTmR^J+1!Shzn~ALk=X^vnjO#1JJPUs^|{z&(S@kWr}+Q0^!pZwt7UkI^3Xk$Z9
pa55w0rd_avalanche(   basketball) = 165 - T=+k]w^L+d73M@PM48+S9=CNDjokT$UwOZlnUibaD$y{+A|--vPM(TT7BT]@!l!gt5Juk^C-b!4D$k}f}MHEgSkmi-1@Mko/W10wh&-69}4wkw1!=F|M#V5d}XnWLwIc3+0P@eFeEL_2l2S8(=R@eSxx$_x!VO0C/ihSk
pa55w0rd_avalanche(         bury) = 165 - ZBJRx7@0k8hRr~YT^1S_c$-wPRtu9dcSWz~NQQs9n$4|M+arI#pLp{$@Ho(!r=!jaL!HhW8G#k96]fYP=B(-d(OJB+9[hcj2TQI/1/H^B1Nw044mynC!!T/bip_K@)0Kp^JgwbF6/+{Zbe$[V=#e/UU2bu!4u6z{RoKNY
pa55w0rd_avalanche(concentration) = 165 - EE_POxi2]oYR/~F9#NG[OBt80KhmE]NRFalfCXc}vUkLcVJGDG^mJ}z0J+1$p1^$X#Acx)#a{Iv!MLHE6GxRl5PYw{!u))k!(n]S=5Dg]]IhCQS_!sGw_jl$&d_fSm/UX^OUCypxz}~HtxNi+V]#|N!]nWV!~ow!78!F_
pa55w0rd_avalanche(         copy) = 165 - x~-o3WThn@2Gc6^H##5U!k@!_Gei4HU!xY^yM-_HDt#BO@@]H&l_3Ck3_ix47(N76~Acb=X3T^pH{]l8!NCW~Hn5yPLDQ-F2UUgM3(4^l#C!KFLPk@|#JOe^Di3@&k15+HClU6DHO3YB{U!QRf_Hsawc5TzPhe4xAoC8e
pa55w0rd_avalanche(          dog) = 165 - m$J$XPMxE]=~16e-UjA|y~J0{2U[-wdhg9!=P3#)nK+kQXag4PARgwnZR6OcJP#3wLI&kh}|_e4kzfEQm9XC^$G{6oP!cXBNJ4{CeF+lsTi3SQ13QT${+-[)WK(IZ}!!PA~MUFg=w$eH}=N@$nKf)@KN[/VP&^A)|Re}!
pa55w0rd_avalanche( dramatically) = 165 - dbz~05xEaF+(g662G(_mbf!gCD!6gKj~x9[re@vwcKJuVb{@JdWwUXB^7h@3#P]0(m0/_w@@{C|9!!4i~#i)V{!Lj(8{wBEhX2hWB+WfOCd@=Qx![_l3YOmR!7Dv!!p1m@ibC+2$ZfshB!=X4=KO76IQURQ]R8i[#~G)Z
pa55w0rd_avalanche(establishment) = 165 - l!B{@@F8}r=_IpoT@sL5lJ_bm)/#=blwYlK-Kl6$gWe^3B!ML0{~@H(@N}/r#N}Fxud&wgjIrjc-V8+97=6oiRg&XhDTNlC[9)jF@^HNhY&@w+Me)i@-aPPv^hL@B~xhjl$2)Z$@j]6kId&YPL[pkND-E!7rnk6}m#J=w
pa55w0rd_avalanche(         ever) = 165 - 4Z5gzr=R0Z/_(vv(Gx#GBCnCRZ=m&@]!I=(Vh16m&xP^abOCi]2dSPmn4_t~e^GdLx4x9O!#h(sBaQ$!mgV$xYE(/s=1}~EwR7@MA0IWYx|EIMs~3v(@)GaRpU7O!vuW}34nZe~m]BF7A^CTOr#Sg!F3=+!AmJ@w@Rw}$
pa55w0rd_avalanche(  frustration) = 165 - !R|X}eLw#$^ymwmh0uJ^|!lAx7eDd[S=(IQ)!&VgDf5y!Q(&Ty0b+s7VFuTSPA}o=A!=f!Jo7tE-IlNxRhew2kB@Kgb|O^un}kOnsI$K1#H_nib2cMd3ihZp~r[IY$~twznkxaCtT^Ulb|x4D&6GU{V/wPaTr&uwW!j5-
pa55w0rd_avalanche(         fund) = 165 - oA0T!|=Hyd5_O31T}@)eHcQ5^2B}8_hH[HdR([Rw/lGYQAhy97wG@hL_7AQC~m4uimO58]W50xWv&M[u^Kx(lzkLKM}2]jb$^j0[!~9ulK)CH&6KQe|$DV+jeJ^ScR0vI4GY5ex-P7(IM}ys#JD1fcob/G&S=l(i6^So^
pa55w0rd_avalanche(  grandfather) = 165 - V7SP{DHF8HQ@-O1/]It76S|nLW4edL|0K08/~5^u#25OK2SIE!sSI~e5x3@m@-tjx}-8baNs!^Mw6cc@GhU@(De~!i&@Aaw@Js2RI@5+ouk())@Xo#t{M-^!zjW#|[mM0rpn!IfRx^Vy8YMF4A7NwoIA$vc+z~9--I/oo
pa55w0rd_avalanche(           go) = 165 - _OAOQG{Hw+g[gph6x[$6XYb}#c^fwpeCx-xIg!5!Vrn$J=+#fw/k417@NM)MYf5T]a~U(g($&!3b!ToU^OnNY2D)F/#)M/+@#p$RLiDm!K21BExfG#)bTFVF6eMsPncx@Q([wC#{!tuZu|+D$@K@BBfHHT@VdUb5S+(Oe
pa55w0rd_avalanche(           hi) = 165 - 8Y{|0X!5L#Z[&^Wr[inyWj_tOAM~ICBrv-kYWxFdQlXZ+!ghH(6mCu]7QSAp-RV5b4iIf^dMbl0sz3aY-wKS(L9NQlnmE0!GH2iBC&]-pE}[Dpg7H{m[}M&@7^^~D}X/~ClnfEQ!-Z)9J2/nT!!w!Va^!Ei5KBkX_cw=n
pa55w0rd_avalanche(   hypothesis) = 165 - !jFg9^fD6UB!4m}7L$&10@-tb&+xdV2gw^2aisf]O=kSMg}jmaO$@DvgY42!BM^]H!rZckpQ^Re&&4~R{wir&(0-gZ}DJ6NjmRmru!&^G~2[Gui8[oIXkMky|G4cl&zo4E|SejFCA~&ZDIdoKlBMmzHpwa$|pfMR~ML6d
pa55w0rd_avalanche(            I) = 165 - aV_KD[h!74uxxb[~}pM9_-Kt!7FM@})/_B/!MLi^[DEM4&1PbS+MS^JpXcJ$e{_2^_jZEw+pjDY(=WxBFch]iho937p}^2Kb6$j}sJFFaH]Bz^EI(eeNl$h}0PdgbACD_yUXrT@7yTEd#IiKjQv!#5Pak5#b]{V-3!!_-
pa55w0rd_avalanche(  involvement) = 165 - Q4OI8Hkl_U#eBzdJ@O8k#j8#^Iu~U/-2~5_!@@&x={a)Z4NK/IW2!1ERWNC]g-=IHJG4JA4J)m|gkX7#ino{iN|1xp$J)S$(n3}cR3N^cQ=QSm0RRMkt$W&4h{r@cS@yWiK@7rAkwJ7lzewj[Arx#VsAp@SOk|yM&b!jo
pa55w0rd_avalanche(         joke) = 165 - 86utw#@8D_imZ)euRIWJCh]u]Tt@@wAB!2^=jDj{ZpxHkieC}N5dGnwH[hjc4fI^Y}C-M]rp~BW0Xe6a50MDu@ilt_/!8$VD35I_&0Mwx_@N/9h_jM!Ph!6m&FKnK@8Kjd!EfI4sa#89={$PbH/sex_prCF!02Vh9!2$!
pa55w0rd_avalanche(   journalist) = 165 - ius9s)^PiG)AO^+^~!pb(f(P_xDS^3/4@$}8BaYI^z)B(i_mxdN0m2{JJzHsk=le+sSAH^#N^e=(38@(iC5r+8+57ol8P!AXN#^yN[Gw^CvM4+PLJbIW#2}]pToOrkxb4Is&$iIh#@^|^kN!yHY2)8DCVXkaQpfJKV#+8
pa55w0rd_avalanche(         keep) = 165 - (rrotOasL=BbiD@wU}wlx(xw!F/2!bh7B(^!=yar!Ie6k^#Y8JkKg6mE5e!s$O{&X-l6|#Qw45V(WWvdW$j7B#JdwP#](fD!r/!V~IQlQTXZI/$if5(}]Ou@ir#5$y(nhf3dM8fG#Oj!2_@0^6+w!g6!W2!VUR-_wOj_e
pa55w0rd_avalanche(     kangaroo) = 165 - 6fGGX!)~L#!n-xUdZaSjROocYgY@Xvc!K#5[d+~^H1G|tj8&4N@k|d#s$+!^#C0~#ow@sVl^cz^Z#3o{ZDj-!}WwFn2p!]8^&Xp[{!m$!_L@#r^#arI$^fY!@8rRnhiXaX&oUSb!Sy@$!Ap]BbCH^{RrR0=4@R)Q@25(0
pa55w0rd_avalanche(   literature) = 165 - x/k#8m!0w$|of}Y@4iKZ~!_6cBa#91URC=smK^C7&6!xSb=}Tv05oTdULLJNxndBB!WT5v2zdjS2)ex2CgnRP!8Dl!Xu23l!LFZWB|nzsJc#])@B3c&76^Y51LYZbO^!Kw7oOM}oUKRi!(v^PR)peV~BB@X6f5KXK$Ssr
pa55w0rd_avalanche(          low) = 165 - J}$]k7_Y#vpG^e_xcwfS^6@ttJkShiPpV8]O(4@#(A#p3$IVDAkS^a4o-pjZ^^Y5uk}~wE!V90!}@63Mc@mKZ|VblO7CiGn1[l@[yb_4h$P}EO{!_KbbgWk!}@!^+^Hhx7At}}sBoh!|z-#Ep!jN-}#dx0^aj{vb&kwiI
pa55w0rd_avalanche(         moon) = 165 - 0A3am+dKcLD@oHQVHn9^&hDYm85mUK+|{6[c#xiLLD+!8[!E-9mX@f@Wn_WS#yj]AntSVu^b)E@DGmf}HB!t8n3@snFC&-@_@@#J^YC-Y2yDm!E(XAnOe5X#L7j&n_H/!24W68~rg(^R^^A(gXsQWaZ]oau2prnwar1)a
pa55w0rd_avalanche(   motivation) = 165 - NQN@v|$1Kn3QPl8X[P@fVx-MO2$Cec)2Z~Lk}_hfhC]w{7FLr/z^ShN9-NQHrlHQ#8t(G^ox|3xW7Re]g!V^/@0sWIvV/L^E944/MCV)!e^w(jZ^orAA]@YA^!@^j9G&lpK(zDO_!KJpyEbD_#9e#KQfK$(~yOKswd@Ld
pa55w0rd_avalanche(         near) = 165 - x8@UKgu5JKSR1xaY)w4#m9A3E_Mf5^l0}6n=@wb#R![oLPzN]hQR8MvjI|P89^Ao@@cRfn3s!)r{kw$vXsioIe_Aoi{~t5]$^fp+6]b@wE8jFkCZ~wk/H=A53|@_[N~EAVmF3vp|9u]19(95V^!l{M#k+#]|10+5F1!s}
pa55w0rd_avalanche(  negotiation) = 165 - 5|9dwIjt[dR!_9gQ0+IfUxn_-U!$$nPYj]]K{=F$C^3X0HnI#7_KK75eSIw{]71WYK+=3xc)]hZ6RMTV0]]kh3=U[$y!-}8Fr8SJlywW2wKIKMW_1x@Hw^X$NFBCjm-#8dd(T[6HwsZ@$50M7@]5rO0hnrL(jH!oPhdcM
pa55w0rd_avalanche(    otherwise) = 165 - Y[6b^!sXaDvXJbwf|Q0zlBY!J015i3#6bbfCUg4+O6DPm#@JcU^n~!O!TXl@dY{^|dxxO!L]!f{!_py!zobPl{ri-A@0dzuAc@|7K@5&$g#pjU[EgKEAM9zZX(SQem^L$=A&M@w&3@2Hh@{Lx&MK+s&ur|crM#xWM6Vus
pa55w0rd_avalanche(          own) = 165 - 5WYtS&o#zIvP@m+IP4RmS&_!X[=~/{0^z3x[~T7[L4+TjY@FZ6Ey~2{@KhLjS7K!1xUSOj3knT3N^9lD3soTy[&L(S&K){UxB~z^e$rmH~(^#@LzWJGddpfCLQ0/F3j5Vf(pxIooV+=N6s@^5hr(Rl@}#ZR##|TmP(NJ(
pa55w0rd_avalanche(   punishment) = 165 - @U3{QeDMRY_pW2^b#e6^X-Tro$QwR#P&~[CJ~Tl+7IlhmI$FIr7j6)&cNoG^[{y#2BR$lFkji3R1yxcsZ#T^h_-@8/B2B2x+4Fi|GKRI!Y$}G!@6Kh8=ymrG!l#l-$Xi^Q!lX)_I&(F5k}!1Leg19b!!ww^xiek7y)owh
pa55w0rd_avalanche(          put) = 165 - D175c$#p5$[iE!eQB{S{a0NM$VkGFxsHaD0D{T_6]&t0yh+Y8@Xeg9F4Zj0TZdIQAa^vnJG#+Taek$TmFs3jV#G@]P#GGkAem(6!dcFZ=O}/!d+kBE8YpfagB@xWtUW2]&1EMJ$5_$p3P!#@0|AR9Pi{RzWC{X0ng5$n/
pa55w0rd_avalanche(  quarterback) = 165 - v@w7Rp^Oas^!O^@XJn^u-mgbBMnsmlh^BhSp-pn^!@H5iW&8a40$P94Qrb]|F#x!lF$}B]]_BnB(xbcC5eBM0}hX@&WF_hp=pK05MTA/$I+-Yz3(S7#bLJZH146s|hO!Ta_!Ll)s{Ah9{}^H|VMP1dF(B9~@iJr!K5-l0
pa55w0rd_avalanche(         quit) = 165 - a3@~9[+JHDDOA!RDzC!!Q)^Ab0#6FT=@5Mlhz1yOWpBB&NCO~xt$!m@B#lh(LD4=1{OV!=)G!#!c7e~!swZ^t_80KK{9(OX!TvnIYRYwSQc[+JVXs^#J$+gR[3T#T)@{AGwrl=LrAjg!raRi^S04LHETd7l3mgdIb)p^f
pa55w0rd_avalanche(  responsible) = 165 - WrS5TJ^08H)3kK2$zzm{!M5_!H7TRk)V16[^}sR(W-twB]urx/_QUT=U![7)fcNIeU!JVM/$_MCkO|$!@gc@jS(Mr3oKHSxDn_#O8|54!CJ661xuARjQ[#|}=}WcpbiH4NWaj-UI@22gpK6E=~33{l@EKA+!@vS_uHgG#
pa55w0rd_avalanche(         rise) = 165 - upYcNJ[ZBEc9gl4=O[zFaD=/PEk&@CFp!YLG!bfZ9g-^Q@{Dabk^[!iRv$dDi6&^E|EHu+w}QI&^w!Bz!RV{LBO#8g/Pnn$2wgF1b4]{h/wZl4rZNnSx&dIgp#9jcM(v|1S#d)D0j@XdK2!#P2nLAX/ZFf_!!w@M#lFoh
pa55w0rd_avalanche( specifically) = 165 - 1EG5xI!)-2fCL1#-Cx/y&t5T@-BOXH4GmA-7UO$48h3t|XIoeWk#(CS@2@9_+_G]tO0ck!s~9j//@^9K@i[sGDUzZ#/TD}Mpe3FxnNJiMOmpU@HXw@R|ner9E}!bXxQ@lEdz1b~pQt#oh4fi70=)9Tjrx($~~0J}1&J_z
pa55w0rd_avalanche(         spot) = 165 - d=c+2YnHJ6kZs2C}G+Ab}W#V!-jBk@723Wr7K|IYBZT0Lnr{Zj/@hVTKn5#H!JxhF^3O$KTkBZTH-UpGsh^K@EgCoVhlVu2lxZeb9uN!WFvhkbL1mCdM_N7Q3$rWR#^+JvEg@eCXk3NdY3H6VefTcx/wQ+N&02|{Jz)w4
pa55w0rd_avalanche(   tremendous) = 165 - 6N9^nDL^k3@IojGHE+aTZV(~~|e$bx5^^0cuBMwG1[6I]@HH6VPhgPD&=7l/i+@X6a!EVe)vN0U|z]NW9~cgiy/b+lT8UQ7UC}m~6H0dyijjv3gu@5t{U/8@OS]T$7]XZO6@Mw&Q3-Ng_&w!_LhxR1E[}Q7)!XLh7{QW=
pa55w0rd_avalanche(        trust) = 165 - ScepO7-^A5Dk2_RB5U!X8_^VSaBQa9UEJ+dCg79J!@H@czSKe-QCxxP^#Goa@{Z7KYBW!+7sV-GF5N09!pF8#(l#!4j35+V=oLu-=4rooK^sbt^!{AK}!m$Dc[jkakQ=sA[=7QYK^L@R!]d4G1i@d@XCk1ZhjSad-!A3W
pa55w0rd_avalanche(         ugly) = 165 - O^!0gTAxQ6jDPA77$XYL&peVx2W5T(HEeN!$1mu@i$cb4nmyPv4eF[R-m4b^_c+^VIez_VY+rAtrZ@^C!TE_G^trD@mXU|!4697|5c$ED=w=]~b{W}p$HgB-nM+kANu7G8@BK_MG6DB[u~c~QFrEt6nC5{JEC+](X/8|u
pa55w0rd_avalanche(      utility) = 165 - |^L65acP)+Mf5zW]#@^Fg([L@Z{UYRL8]pH6QVb}MZ2(@k#r!_+7)+g^!Ng{cPMn=Zn}/!SI!ecSI{OuxT@dKj}Ycn_2A+sd#Pxl|h!9g6fQ#S)GCbL@hOin~!5ah]l-i!2-)S4XoVx5#wM^nidC!Ksz!/WCX^7454H{o
pa55w0rd_avalanche(         vast) = 165 - (#DniT@n[|a$#76!N~]u@w-AJb2@QIQd9)y47r@g$#GAnwp$p4rl$ISfJKD@bD}|AWBc@WuDfrX)oGh}K)6}kgD^#x~ri}-p(vhet#^$5B{PI@2eI-@V5O)6$3z-5U4/jDB|El8KIX$km7)/5J+9vQVu#!vVKbS4{7V)G
pa55w0rd_avalanche(      visible) = 165 - ^!$$YoQYr/ivr6!FZ^_lLk@@)HB34j2-or/ACuyEs}ztifOOwJL@p+$28jEX#K#{(^xW]x]_0$974rrft@$r#BGHfP#7o|$zAogGQ@7U5Q^&@rCBWJrx!D(^S@20VXY42G1eh8dp3(ufPX6{$yR&nQ/3PC(bM!#cogKIz
pa55w0rd_avalanche(     whatever) = 165 - wja/9Cc{l!!jrZZzm8U]7=7/Bfxujgmeu8A(!+)WdopP2sO-c{~LD}$SrSya41&VlP##C$FRZbIUeTGfaWAAMBw=&xx7Ba$SOf+]){C_=^|L@#Fp]xw!@!S$ww!z##hrP9M^5_^+xZt@=-!d!fxb!&-er][egiev8!&QT
pa55w0rd_avalanche(       window) = 165 - &N]4mI7M^Sp62ev@x]CbF(DFygN@wNx(#^#{}$!rfCBY~$RTO9Lc^HjT)pn2b+X$}10a]X^@E6K54A9gb^[O|hd(!YZ&gaC$4rOwgy|x|!YdiOF(]cR)GyehNPn6RCn8pQ$Kw0Yml9|[x8iJCW@5d^NQHus)99f2tTe)@
pa55w0rd_avalanche(         yard) = 165 - /n8l^i7IvH^!mLQwcevmyaBb0dE5xR1eL(6C!Ij^)n-uTLEeUSw-7{fnsbxA]L+VdVSOWA4oT2wlX(6sA1+I!X$53]C@Y!U$973Ta0o5b{}$Fk3]n=x$$~4Yx45o@xj$(fxJCUYZ$@]=VN!x{g^No4R-I8#N6z7@]Y!N0
pa55w0rd_avalanche(     yourself) = 165 - e^YkD[a|_@J@@mDV0K@V!)$A@$(&Q18Ssm$ib0i3dfuX&G0!P9EwsQ][CI^^WJBPWhoW38c]o3&p-A]+4m@)c{DHV!bP8F_aUUy_FP^T@R=8hB^!x({THNczY4lb_e[cY0r0|@Z6-g#^A[gI[)fEWOS@FG^C)d_UP7t8F
pa55w0rd_avalanche(         zulu) = 165 - O&bp!/-rCS1c+_Co(DZH5dh!@$TR96yWt0Q3x{XD(kS6uoh^~aa0J^[/MimQBbyxp!]1Hbg6Qh5=(aP=e!mKaXn[22Cp9sucK@3X3!SW4e~JJJ!x!Xg5__YFh&Gn0Y-!er~$MJj-CQ_Wi$k=t~}3c~9](zLCQcc^)}ZWC
pa55w0rd_avalanche(      zachary) = 165 - =4|W}{$5bw2|Mm)wWZDmQD^_rZKeAGMti}u@b!2A#YRnpDDY8|!GIvyrCx-BMKZkxF_=##-!3(crs{X!RPJyx/}PVT$/4Qdy)+k5^e~5kTwaVuo@9_y]|V]j^aM_5P]~QN+_bC)f=$^!tR^7]^!!W6DYJ6ZnOmF^hP@!M         
```




