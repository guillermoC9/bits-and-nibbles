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
20 - Vwk4FCMWPU=^Yam|V5Q=!

$ ./test_avalanche 61 popeye
61 - Y6gAXZ+ctRpx#es8o$U-)F^Px]#m(}L~CLG$$~N{m79ch/x^^gccV]#O^O}()f

$ ./test_avalanche 20 popeye
20 - NvhZ@}}p9e#|)C/|AS7C)

$ ./test_avalanche 61 popeye
61 - iuC0}Q60!=#bE&J!lD#AH#{RZt6e&!]g@P8^&TeVa!@vWME2mv@9Ch!boPb{Aw

$ ./test_avalanche 

**** Test of pa55w0rd_avalanche() ****

pa55w0rd_avalanche(            a) = 35 - I]1$/z9@bKNW^r)|dD]fB![iaYRgXbvsE1@X
pa55w0rd_avalanche(   atmosphere) = 35 - -IeFxk/!L&vLV_PENSsaWPA/@aU2+X]&{jR#
pa55w0rd_avalanche(   basketball) = 35 - 8pdTJ@of$HXJw|P|t!$@#5ytb|Z|@8ILP/h~
pa55w0rd_avalanche(         bury) = 35 - bWmntf@7TR=h4E3nKy!!pA!!BAn8^FYEd#EN
pa55w0rd_avalanche(concentration) = 35 - IuArRFtvxfl0zFUDHepu#Z7|AY0fpjL0DQMg
pa55w0rd_avalanche(         copy) = 35 - efz6)ZNuKhC+|6^ra!~a5@kRM(mU!tKx^9&A
pa55w0rd_avalanche(          dog) = 35 - ~!Ul!lCb#45sV=@)#{G7Bb/uZ=!3Z]0ok6A7
pa55w0rd_avalanche( dramatically) = 35 - xJtu$j@ATb/SU$Y{wO)u@xzArG(^^ZaYcdjE
pa55w0rd_avalanche(establishment) = 35 - hs@5&5lZ1H0S_FTw06n]R${b)i{xo@SMAbhn
pa55w0rd_avalanche(         ever) = 35 - {Lk#41!7Q]#~coL(]w!SejbI7^s9@SOZ~(gF
pa55w0rd_avalanche(  frustration) = 35 - D7=gJ{@Y@O|OVgbve3S-#I+dC_lYXMpKxn@w
pa55w0rd_avalanche(         fund) = 35 - Ol0[ABKnh-w/l49#xB-c[FGYJN_9)C^]m!Kf
pa55w0rd_avalanche(  grandfather) = 35 - s+D1zixVM^zhH@BVm~jCI=KHg@xb$j@+a[kv
pa55w0rd_avalanche(           go) = 35 - $dYh(!vnvAA/V@lr=g[CJQL~7NV_GRg{N2tZ
pa55w0rd_avalanche(           hi) = 35 - Mr|iS[X5-Azao@&OJ7og^/|G^vE9~&/4b!4@
pa55w0rd_avalanche(   hypothesis) = 35 - @o(MB!k2lW!9h/N@cpFP+yr!@0m@re/5ka9U
pa55w0rd_avalanche(            I) = 35 - gKmL!Hl#$O$zxDf5{[5v{@yMp)EC2UsU@xS(
pa55w0rd_avalanche(  involvement) = 35 - IMU(QUe=3~TzNem~=Uf/UM9iLx@Mo~j3Wo^#
pa55w0rd_avalanche(         joke) = 35 - Owe|v={A9b)f&2Ol!-DN!i@!W-X4@NF2NH/h
pa55w0rd_avalanche(   journalist) = 35 - 1N8x^^hW#d+h_Gt&=$$(c/O6#!RH_v~e)OFJ
pa55w0rd_avalanche(         keep) = 35 - U|g3gBAxYwH07^6$EHs[|{lw@16cbr5Zrxkt
pa55w0rd_avalanche(     kangaroo) = 35 - ]xm~X!G~L9K/#tUQ-D}x75S4YBdMMeJ!=bBF
pa55w0rd_avalanche(   literature) = 35 - 9kSIN(Zmw]pEy2@u}TkR4$=r$QM!/5Aing]u
pa55w0rd_avalanche(          low) = 35 - AiS!1&FC/$=8RY@m3i/@h|=$^!@tJh5^Aorl
pa55w0rd_avalanche(         moon) = 35 - r@-ZO2W_{h#orknMSL$mpyw)Af@6BeECy@nP
pa55w0rd_avalanche(   motivation) = 35 - YxM^E3Ye0WdL@0!fTVjQWiQeJ!|v8Lb~J{Cm
pa55w0rd_avalanche(         near) = 35 - x@2EJ/Qm3eC6{2|4iXJ(@D9DNm8_KH#o@otN
pa55w0rd_avalanche(  negotiation) = 35 - xmu~H1ia3BN(xic=HJ{4~M#7$~#gK6HYUrT#
pa55w0rd_avalanche(    otherwise) = 35 - !|dOj2moYDy#~FH{x6Tn-yC/0!uo+5~2}{7~
pa55w0rd_avalanche(          own) = 35 - GRLIE]Z4nQnCxBwp^-69cV^]}QM6~!~2v!c@
pa55w0rd_avalanche(   punishment) = 35 - 5!fM^($@XIJn#l5@!4#gBCspInl$##O4)mip
pa55w0rd_avalanche(          put) = 35 - d}m#|p(9N6pakTg&rnGDmg2#PMUN(U^+U-r2
pa55w0rd_avalanche(  quarterback) = 35 - Z03@Pp!RkZ{o!hE=dYwFY$jXtgA(M]!!z{ce
pa55w0rd_avalanche(         quit) = 35 - @VhR7v#N@CLKbwX-rRZ^^ShfGQQi]W}W$wdb
pa55w0rd_avalanche(  responsible) = 35 - PvT~r)91]DZ$zwaoE3AixSD^C}IK=o]kVw6i
pa55w0rd_avalanche(         rise) = 35 - DbK{@#P$S+_tu)T[PUNbd1gfc64l|@NA5OJR
pa55w0rd_avalanche( specifically) = 35 - v^Fslww_(Lo/8XN@Vg&cw-rW2SO~K/=uTdxO
pa55w0rd_avalanche(         spot) = 35 - GaWsgj@6INesR=-5c!f!cVxNkQnT3!VsFR3/
pa55w0rd_avalanche(   tremendous) = 35 - Vg!{W/FtmXe3frlYN6VElTVxcxT#C(7M4Yn#
pa55w0rd_avalanche(        trust) = 35 - l@lbD4r[bQ{[-Vi_8mn&=tk_nteP243d/w2J
pa55w0rd_avalanche(         ugly) = 35 - UN3VrPP[oXp7@w8^vdK^c(F3IVb^g-KdV_V7
pa55w0rd_avalanche(      utility) = 35 - 8ba4g5+tWiwd8N3TenK~[dDW1SN$zrhIXnkB
pa55w0rd_avalanche(         vast) = 35 - afY}NhG0]c!9P|6ddfaLO@igwo$2xO2x^@#0
pa55w0rd_avalanche(      visible) = 35 - So6oLU(](5!7SgxEf[r!9c^7#I]pc+I4r@~!
pa55w0rd_avalanche(     whatever) = 35 - &lc248k!-o/29L(6ay^6KT6!_i^/hT2GpHHk
pa55w0rd_avalanche(       window) = 35 - Ha8Lmm}]z@s-!WKg@WkV{@jexBcp-y!2=Z2R
pa55w0rd_avalanche(         yard) = 35 - tEbx!7!)bti=]!meN~|KGf=X!r!6Dom$EFnY
pa55w0rd_avalanche(     yourself) = 35 - N](H9=^mlm_fSwhnD3^4pL9WM^6![ipP9Ork
pa55w0rd_avalanche(         zulu) = 35 - bov}Mck###V!@~THAA[Yc9G#[xg5uWvza0-a
pa55w0rd_avalanche(      zachary) = 35 - /K7ckpBwE@c|WbPR{phFwE]2!xI#Iu4W$ym-

$ ./test_avalanche 165

**** Test of pa55w0rd_avalanche() ****

pa55w0rd_avalanche(            a) = 165 - t5o_@lc+!)Ob{Gb7^$DG/TB@INIVNay7YFm($I@9TU-=-fw1(#X()!Zxu4{=3!-JUhjIEn^c6E/BK@d-({!2!6^b8C5wgxD)pm#z$cQwV6~&T~S!#8=2$&@$/a^#_W4s}0Bv4SsgmTn|1!^!wR7wxIJB-F)rSH4+t&(!H@
pa55w0rd_avalanche(   atmosphere) = 165 - O!V}Z_gVirwxfIUaEG#E(s8DO^luw53ge=8Chd)!5I8Gj+isK5!mE7p^!(7BfyYg!jzpic1RUd#i$}v!R@^l5LLSH=anSQ~d6~eb^yge0CHpZ^&GwNFg@lW3F=M=^w)DDuF9@s&s|QxD!)a7!Ru38IL{sDG])p1K-K=]PG
pa55w0rd_avalanche(   basketball) = 165 - W2RhjVd^ZOkjFA00YJ]}}-4G=NS8]w(mT_3$w9!Z2]x8{Q3f@z/LBhg8eM]S=|GXQd|P1wJ-a6/Op!pUI2@+T1OSGpt$W!O3|G@=j-)G@o8fg)p[_Gx-bV@^5!NEdl{E)Ik987#pfRTPElT/f&H06$Pu-#rtg|Q/~YZ2SO
pa55w0rd_avalanche(         bury) = 165 - [+(IMXaW#8sR8YEBRO$dJr6i[4E_|4-QI_r~1@tpOV}|mY(5nl$QFPP]$[28y^Qc]f_h$&M1@|Q0WKB=|w@GpcUZFC8]ug!@EI)|ID&3[^QmE]=#yO+]E!r#l!7~0NgoDe4@n~JIhBC)Q@$XtMVYxrxRG!p&zw[~6k{b$H
pa55w0rd_avalanche(concentration) = 165 - g}Ky9k3!&I/wY6T|G(X+NUk}!laiKP@F!vHE+flvOGy63=#PZAvM_H6|~V@HCc)oKyG!k|JK7NwefU!Bd5Z+b#Q$@4!wJ6QtM5&YWxbOGb)N1~3O-IyAQ{fwH@bIDl$jD]@e#!nZ6k72I~#UCd$kRpD!LSaBfYLB7rxg=g
pa55w0rd_avalanche(         copy) = 165 - JS0/U@^LJY87^LOyxo$R#+0/S@^!]igkFS&|j6MiPfZ96fNu@-HoMkYYeNd92&B7aE_TOEZh+KQkX#=^7+u24gs@kL|Zx5H!r0#+H-6!$7YZ-~gf!&t+#7!Cp=0DD#GxLUL!cPOopMVh_X![w$7fe$bO2HWN6R@@[@j-7f
pa55w0rd_avalanche(          dog) = 165 - &]H)wc=u8D!2/gM}T+fZGVw#@A^}HrbXNLI#!pQczcpLGKS!Z^~^Ics/HIfl[Q=78FbrCx8oK-PnR42aV!][^8@cbK@h({ZT+WW{e{~m^kp!!$/lDr}-rh!~1x8V64@L/d-+3x6utoMZ8{OLaT^J@16!Zm@[X4c3p{k^p#
pa55w0rd_avalanche( dramatically) = 165 - X@8S4#/VkW2yT^ux!mtW^D5{S__m(zEh@FG@ep4Rc|IyxBxpj3O#=P0U0#$(~n~--@}mF=/DzJEfGjW$XhNw!f#aPQH07ape]g4B-|s1$NcjEf@D5VsM(w=cw2WSd6@(eX#&hQKIODnn$xDs2uNRx=y/5_kMAB3@Pe#]@_
pa55w0rd_avalanche(establishment) = 165 - tXWgjmM+/E!&_][!$AXGl(-@&Mtt3Q)!}/3{H!#E9fEZg25VD[LFQ0_k-}jDgV@H3rmF@09@$jMHunhob#]|E)]x@yMbs}#xk#N(=~~H-#bU/PBMEB@!3D$PX}ExGJs(_^Pjb$Zuew^!@+!+OZbIOGIZ{da7pe^0th#^lC
pa55w0rd_avalanche(         ever) = 165 - R}9)5{EPKgdwFQTSkUIj!_80e$L/$uHeB!tbR0Irjh_#OnD7Y/j+^S+rVgx@Mj-}kxwN6}S#|DFOIWuH[O-gGixu$rH9R$~Z4E{ASJ#V6t|Plov#KYtC$U{fd8|~kC$Bt!HFbnh)9Js!c#^JM2K|c27MZk!(^@P]yP!MU&
pa55w0rd_avalanche(  frustration) = 165 - /zgUD!SZ[rF!t^~Nm7i2AmpBU]g^ubP!^$hYnO93m[0o!iI}Hh^M/n~YX0vCcm}f=@2Tz6&G#gMvv&-h|}YT2o&~gxRalc/Z=_XMFH#NJCET(x^{8dxc3!@asx3~x6wBVI/o2!C8E^Rwa-@PU~5=5uiYrAHrua+$&@ksjW
pa55w0rd_avalanche(         fund) = 165 - 3-aR($k47y+xe2l9pG&#$Jr~S^p!t@#+SudeC!@U@^nD}$D6z@JO&XlEWRaRY3nnai]6L4@0wn8F-xx97@UCEP#Q/j^CBnb^g7($L-Ta(!ty1GK!xxi@ZHwBO=U=f(4(}}iwfO(zC@cwd#|@j!9}p$)!$_=DNsK5ynwA]n
pa55w0rd_avalanche(  grandfather) = 165 - ^)P!6gEon@$!}nYl^}/SmA9jB#YFWbo+!^^EA@IgS7G$$FW5e8ae^na^0Qt!Vu7=&]o{1nl&7Q@t(1}]!RmgzF!NTk]yrc$DbH0CHFb_wUw(NR-&C#Yy#lj[vbOb^VoQ{rO&~TFRM!OAc{iGTE0o6c/R)oh75/D!U/DMPe
pa55w0rd_avalanche(           go) = 165 - @|VnmWK@bJd/MmgKf0+nPwUIX!Vxt4aL$=x(!Iwx=7y-{~G3$|08DAxWISd!R5rfnk09@^rkz/JHud_ii9}2t#4LY=h/B#wBn[u2[_O@$#UYgblwds_JW2+#7R^W&@n$-uiGWd|8e]DboLZ6t!F-{Jx5XH$/vf{ML!U&D-
pa55w0rd_avalanche(           hi) = 165 - K+ySeTaG{@@cIw!7OpR)@1!dV](t@#4L_KC-p=ruJME7fSFbXxk!W~)M^~{@8^6!&dx{!Tmy$|/@R2^!8^e[H9l7!C!_w/Nr9-/w~7~&#&W+dJkd^t#ZpTs4Z&+LL@_Y}zGKYT/#7I!4-TQ)aP!CjJWm]I0EEU/AY0a4!d
pa55w0rd_avalanche(   hypothesis) = 165 - aN!xwvY+$wa8vjXcFRi!lKKTDxxe_T[)Fs3n&kga|mf16FuH=oxI{P&7BJ|C}7(BpkT!pwNx_gxLYe)+1A#+rbo~@OtI(1@1)^7SntT}#!3}/(k#!c!V^hrCPws_!Xa{75]SACz^6(w=QdpbYl#)9Ukg9[!JX~}$vZw@A_
pa55w0rd_avalanche(            I) = 165 - 3ngm]]!$T!$Q8w)AiYQM)a=(gJ1#S_LP#3w3IQ+x7ATlBl|o[d5vGRdUehZ=$=AFb(_E/g77nb=Zan)5EBQiGh@e#83Zi=0@#/(ru!|ikRPA6!^4JU@9G@65||~E8c/iYDp1Zu&O8b-6@R0Gm8@ya@^_d^(DP_eRjPx2x$
pa55w0rd_avalanche(  involvement) = 165 - 54i-UxJGKi7U/&v!^4u)lp^k^]-$7L^a^R-o8+SBJ@ce0Im}NS$own-ep}}!pha$k1@#w7]d@V#2[Vv$$OK+[b$FWP-1D&4x^Ld#k!UW)n3A5ijf-H+x{dvGA^iM^EK()^^z5Y#@!keJ=)TjiTG@tk@dt3Yg&CcD59CM@V
pa55w0rd_avalanche(         joke) = 165 - xCXdx#++=2hDSg^UZt9CEa#F{#P4S2L$)J86+cQQ(i!cTNWe4)dIR@0Z{Fk7aO0PUp77fBU~UXi2p9fFw@bRUoUc0n2Dp0XS31oCi[X@W[lSybRz~fgaSbxwT#f~!JXSbxIbix}-xXI$G^flnaT!V)4u)iZF$MDa0IaG9}
pa55w0rd_avalanche(   journalist) = 165 - H6^C@O#^xIR#+x+G~vU!W@Wp[4@[cr)ka-5/zMl$3$4X$!$|/h@Z}3WCdlhyni0h{3L!ziCrj_wE2!#7gya$Zh|V$]n9wC7Hm8oW1D(2~f=e$P!]U-I=Ol^eSDBWoT}@E7v&c(PUf9#u8}jV{(!_jAfyxU{N)H+!+x#_DJ
pa55w0rd_avalanche(         keep) = 165 - @^J3f^Ao!Sa!Qn!FO_H~nD=}7V9K[AVKQhC@Vb5P&9&sh#L+&&D$&+[^0M+b1Iy1DWoA[kbHyU&QZ]@i=XJGSFD|@NE}~nc!B!jIU30E@J=4fCOnw2Et/BrTL449[vX4kaSI~i4yP#UKJBO7m+OII$B2AkZc/)9@{nRgxt
pa55w0rd_avalanche(     kangaroo) = 165 - oT!]P{xCV-G#8c!-h!|Z{-yJSM!!xT#p-L2rLpt@ZX3bjT5gxfV{1#KPQO-a/gb2$~r&=PEcO~Tk_(YaZ@80@v@HVC#@BDJQrYB(@]SO6yB0~l$ox=#T5|3Wn^XoXpTh]Z!2he2^1J1]tni1bj)#}zh0~/Jlz0@WaLFBwL
pa55w0rd_avalanche(   literature) = 165 - )Z[cQp[z}tmvA-PobwdepxDGTx+oa!=Kom3ma=xcMBCgmM46Do87E~|Sd!Af_wO!z!{WsL(x20m0FvotmjulX#x$5kVn!f1dboQ-]1f6H31Ixj_mxHG!X&!NdaxK(@Hp$@d@|{=ilzxDhcp@$nP]|~2$HGrO_phurmXLR@
pa55w0rd_avalanche(          low) = 165 - f5y6@0Yw1{rki!@EZh8m1m=m#^A2@v7$oSMR[U_1s0hIMcvo~G/(NAb-!S0KPk{|w8SeaF_b6M]-H&L]QZS!C@UpDG3l-P!#3S+aJH^3@Ipjn5^ER&yVu&m-E&B_nBp+jctag@1aXuFxsMYI2^S/]BIR4WiB}eg$LB)i^#
pa55w0rd_avalanche(         moon) = 165 - ~Uml@(}r[0#x}8@bFelKx{wQpb/uI0#y2@keCO0N3|pt|WTTT&ZsC3J@lw)c+djDnJpY78)Fv1Qw5wcPQ]^l1[$BymTkIHTHu!Jd)wSdXA$MG3_d}fLExp!kckR[#|uVoG($Bdj7am9={Y3H+bu^TH-$!63hc~+=C[MVNm
pa55w0rd_avalanche(   motivation) = 165 - =!4cO$JYgrwS[!@a+!I#]Z#z#7O6_8##|/ilNwXO9Z^||BYpnK|b!o$rWoK[sY|C1}~+$!Twa!(@Eo-m0PGPfPa!O{{Tz8y#!]F5j9Hnf=|U6Fc(_TH-+SJ{X!Z5X_E@^_@VzAwIxvgZ8jTFX4e0j#$!#y9n57@yF#@9(-
pa55w0rd_avalanche(         near) = 165 - 9!&UgRDQprM!b{/#W2YNgKyA!PPBN){/ATA&S@@_/wg@4Fn6Y1e@]^H6aGw)#GUhJZZAhH{#XdtYRuO-KWI@1sP^7^xZpoaTQ@!w^Frhw{o@L#|ep]ow#fQ@[X#7xQw6A=Q{Sj}NTzwztIiW$!-^|jE9IQ[rkQZ0F-FY{j
pa55w0rd_avalanche(  negotiation) = 165 - p^A1(J$Ubj({xj+!$zNW(A#a2NICkn(Oe5)w9##I[/W!+=+fcJ&nK@/aC((5~fIOg~xQ&pGQdf!@D#4!^EEihLO@!XPIWP(X|!kf+BG^9MkJ_aNP#HF=I|E&MRi_-)n@s)L/)=5]Wl9-WR!1[SV$D~^}-cn9#J1mZu^4rm
pa55w0rd_avalanche(    otherwise) = 165 - +^EmLXbjkcg@x#pMV2&A!ZLZMe/+rr7uGBTm|^-$Fm$@+W8@Y1)@)+VHv]76f+]lecl!lYbbkQ/M6yW&IHXK2W8gUd(aO5M)FBGuvQ9I4V@D1BQ~150RE!ulLG!SAF2[J#T7[!6x6!kSbX2m0m0#=e{SvYimx#K6Jve@c6
pa55w0rd_avalanche(          own) = 165 - ezp_+m!]!4(L^K5r2&SGWSIPUXh6I|I7gi9eY2$[3mn}a#jk7-b~zLM6@M4~IY/{9Za~t2/-wB=TTW3da^haJxc3^9#A!@O@D)cM&sSm)o0@DiK}CZ7oO!!()RIL2)_p(x[$/v5[@P=FK=yQGo$|BoBHU7RO_j~eo@p1}t
pa55w0rd_avalanche(   punishment) = 165 - 2=3#!6kc2/F_#2($&[#t[c@$rH=ndryy3O504P$3R!H2-D5E##1jZNu0J#2ei-Hmw6@63~rt^!P7Jr#$(adC/Rc=-NO{uO#8UGXhoo-aJ##o1&WZ!X_-nuo$#VkgNI/=hNY/CerX![z7)]iLTFmwBLRe)oKFC!#8DcM-d#
pa55w0rd_avalanche(          put) = 165 - LnwgJw(h$ggH7uQ!_wmgxsLQBby{0cr{/h6Ikakurxi!2!@2~Vs1HN2uf&}bY!0=EH9!O^B#b$l[yZwZ/ZbeB=-iYArEU!JP$m@2&@i~NYPS!r#)ca=cBd!4!nl_ozj(PJhZRWtvVO9zIS]Tnxy_AL(U{6Hj_x^^yB[4@7
pa55w0rd_avalanche(  quarterback) = 165 - g8|w/mFwYkpN7B33]8L=nCn@6cQ$[#&r#&{/Na@OE&ju#Kl!J@BGw{xpxpiVzcy+z|CPx!IwU+!nvM{]@}(-j5#ykLAE=Wp=)[)lH!=XYQ)h0Vekv(&5pBrKak9I#-prtjd61h!j$07xQOx$!eM!b([sY3ATgus{e@Q^2/
pa55w0rd_avalanche(         quit) = 165 - #r3xV~^B^o!O()Y9wkQZ!oOp^aHcF{A1l!g1$u~Q$Q{@wMuLBeO4Rj~|W4^T74HC-_QF4#(ByM2=#8r~Cs8{6YJ/vI@oO=!zU5OOxo9A-}!xkz~XSWnk~X6^/pS1/[0$wR4Xa|hzb@pYJ3FAi(0S/1!+1$+Wa/!mK5g8DQ
pa55w0rd_avalanche(  responsible) = 165 - j3nLKP}Ggr_nj]h9)aH5!|#rlK=cf!Dfxea7klNBW2us^6(-7EZL~cTR{bCQ=(goO{w{^p3c$MiM4ZoLd}9}@KYjILG#=wK4(R1|cx5!^/wM#vC^1(8SC^r3t@jy@97ju=(w^YAD@DESwAMteDGk}P$&2[hzV}lW/K3@XG
pa55w0rd_avalanche(         rise) = 165 - _aupAmYQZm^2GlX@F]ZD-e76dT@4[oC|@2Zm9^7@Cc^xlDJ^^a@j1epElOUom!R^F#WP9IP(&pbgFc7$@t][F}5b]i=b_k5~$p2x^SXLGD2f8!k}[~Edk7l[rN@LeFAh#T[$NACieN]5A026Vox|4T[k&U08G$]@9^U80u
pa55w0rd_avalanche( specifically) = 165 - w$YJo@=hO_(/w1Y!bZaJBWX7!H8Te-FUP31@)xCr+^Bau/8^NbxY/@CEF-U2(3}!!erI7QwZ|VGOMs@9^#v60D&G4Al&PvDxS@!O1!2@92CD&I^_r^oc-OZ^@aJK4!x|#_@wG#@8#ik]U{72TJM5tx@)FX2-r4C!dWfL!]
pa55w0rd_avalanche(         spot) = 165 - 7kWn_@mtbgH8TWD^!WvE@i@98Oxddw^^GJweP9=+LGPw&@/MgO]TPKC^4A#$&T2Q#k+1Rx[3~z6Aj/HUpoVa=em#DIpOvWDi(-|TikNhW!^E5)deh~$^=RxW}l#ZHo_xY6jR!]n^k$RuKXiTPY9s!3hDpHZ/)6e#W8m7L&
pa55w0rd_avalanche(   tremendous) = 165 - 00EEJuEd!R~=xy^{^6k5[WSSY-DxshxD/$!(Q!NI3J3h$ZVT/OOsZ75&gj$]n$3/Uwn#N[sUE&_5eBG]Wwf0r=R$@]B_9v~P)y&hhk!V8S-M!@!&VJ41}{Zj|ld#QVI)wRP$o$aH}p8!^EX!2+@j^l31P77d$(zEGMpt66
pa55w0rd_avalanche(        trust) = 165 - Ffjm$ErH#!!(em)M!S]jHDw@(]L5sV~5YQg)V_mE!A3A-}cIEp|$j]&!a0@KjVmH)p~$=NZP@-7!B$|2TlfG$(y-_ih@I)}=3!^~Zp&WN-/}VTv4!SLt#=_pbc^!6c!4}^c2!ERxP]5U!Gpy5fclK9[#+9R@~gJHn!X_Z&
pa55w0rd_avalanche(         ugly) = 165 - Jh_#Ngz(#gZ94DiU$j|YvFQ$!yjIwefr!eYxW@@08@uO1@c|rF|nxW9ZwD&bG5kK!Bi@K]$JZ4tBRv)M|^A((M7BF#TgSswI9wX!UJ^I@$8@Fd1^8Go@M)!m!DJ!~wDZet^^D#S3wxp^ITpw=A8mWvdU!n&__$56!LcNQ!
pa55w0rd_avalanche(      utility) = 165 - J$MpuBclXB!lNT#6@+#nMNt@oFx]d0)5d__120!VE28J^Nr#h31G4VNGy/NhwCg5KpaJCd/9li$4j!J~FDo=EBy(wKs)4^}(JH^ZX}=39@7#{Y@b6W!mxueY-_6@aX(!C-yRHO(RyNz!A@N/+nHz5Hm4!S=6@Z2!~!@!-1
pa55w0rd_avalanche(         vast) = 165 - me_{ZEGA~$}MA2~FUrSr_rAN$&$vI-Gc#gT_l@uj_GEtr=BBc@NZ8L&XhhQV$@gn$!~6eiaVM$al0T]!8^b+FPgdQa6A-c{(a-I00t_oQ-}_Mxh+!L3/osb@b7GanLbNJIZR[vQ87e^@{Xs94Ayr$kfAoHVIsM-@Pp@MXX
pa55w0rd_avalanche(      visible) = 165 - SDaeH@mStjMxLeF#(@~BKHoIT{##$^3#HUZTsTdhJtX5Lf#G@^ACbdtjaB2~XvF{d2NFdpDD!Vdkte1+j=fGrdpx@k41WUH+sw{w$dJ{zw=FYuwB@@oDkaZR6V3983xY/8p&$_B-4#&i05T_U0fA#bLjCWO}Q$)X@]bMlE
pa55w0rd_avalanche(     whatever) = 165 - K7(!Mu0dRbk}MURwodo($x-dwB({-$lUsxo_&4-ikG0]xULY857dRvN(rhV//tABY9-FTs[$)-r^ma[PllT!VLD@9XpZ)07&Z5!{)hW7~d4^nkPO}L})yhZie+Rpk^DnHQLp]^}6v4XB=8a-v#C44o5@IIX[YXp@KUB$@M
pa55w0rd_avalanche(       window) = 165 - LMJ~^lT3ym_o+&BGoGp1JQncyHUxdPd@wBZLVdUWJus3=QgDSZlJ5rx=Jh+UdwDDHMn@(r@UEykm!-URfk$COurc#w)NwfRox|!Zv&0UG9/Xo#|=]hYe&XHJ!(ClU$$|+2D57!ALUE2mjoO!@49e-KcC}/f~20(c&/$J!^
pa55w0rd_avalanche(         yard) = 165 - 1re$[nn2X_3-74Gffpdfiw!pQbkwLhUUo^8&WSt|/#$3fT@Dr9E8M!Zu$NMjPXJJ6RDZ^N3UYTIxg}=uFK)wY9&9Om@^G2!!~zIhIYM}[!d!Zw@7Q!6c$|t!UJa!N~V)j##10~9Y=x5c@)iA#K_$@JF6YDzlaeCPET1)Qr
pa55w0rd_avalanche(     yourself) = 165 - 9}nXBGw-7RmeZj|kQ+7[4&D+$8aZcR6~Kc#Ck/|hTuARd$s=wj)csRf6oL(mFeIYTn]rO3[!_wCao_B!uZY6GGI}ReXTI62J+yhGlYKkhnM(-Yi0epLLWvn$RS!cw6]rU}Jd5mh)X~xf@9-cI+Z7/M$M_!ZJDZsa=MAX/J
pa55w0rd_avalanche(         zulu) = 165 - RQn9Wr0r/2]!UINV{[i^h(6(!m@1#|xKImw!p}hmWrzouk!dn#5JS5Tjbr7WJeTHb4}@^T/I_j^YN14ziA@^}]F5[zI$#E5G0Yuj7ew]XEsT!Nn1aJMF7]KuUHD]iOI-Gm+or{n5#!uls4P!ziyLWd+K^V|^07_o|jJf^i
pa55w0rd_avalanche(      zachary) = 165 - e+s!5pr!-W$lU&D[pLL=R-3ATEOJcg[xL!m+!sfRXzv|YL@YTSNIWp5)^(!$SErxWohaM+$S#$#OB7A!XD28LZZ]-8pC3i6S!0KlNt0Ma!=V#X{|XKJw}uJa6&}@W0khd]_@^8$sUmr37Vb@Yh4!^aI-3TTn2O]MOXdL4w            
```




