# Pa55w0rd Avalanche
## Generate safe passwords from easy-to-remember words or sentences

- Designed and written by Guillermo Amodeo Ojeda.

```Pa55w0rd Avalanche``` is an algorithm I invented to convert a small word or sentence to a huge string of random characters, in order to be able to use passwords that you can easily remember instead of using random chars strings, which you have to write down and hide somewhere. To me this is the problem with password these days: **Rules make passwords too complex to remember**.

My algorithm works by generating an avalanche of bits from hashing 64 bytes of random data generated using a pseudo-random generator, chosen by the first four bytes of the key, and then using these same four bytes to seed the generator, this 64 bytes are then treated as the first temporary hash, which the algorithm repeatedly hash together with the original word and converts the result to chars in the 'avalanche alphabet'. It then repeats this process until enough chars are obtained, reusing the 64 bytes of the resulting hash as the temporary hash for the next step of the process.

Therefore, if you feed a word like ```copy``` to this algorithm, you would obtain the following passwords depending on the required size, which is shown beside them:

- 12 ```F1@oQrxr[c|i```
- 20 ```F1@oQrxr[c|ic!L{rd!h```
- 35 ```F1@oQrxr[c|ic!L{rd!hS{@y[@VMN!F=UB#```
- 55 ```F1@oQrxr[c|ic!L{rd!hS{@y[@VMN!F=UB#|[b8@#wBdC^5!rUGkN[T```

The generated passwords not only fit all the chars type requirements, they can be as long as you want to increase the difficulty of a brute-force attack.

Yeah, as the algorithm is deterministic -the same word always generates the same long password-, an attacker could brute-force attack your password using the algorithm to generate the long passwords too, for example trying by avalanching all the words of a dictionary. The thing is that they cannot be sure of which is the correct password so easily, as there is an extra variable in there: you can choose the length the output, so the attacker also needs to find out what is the size you used on yours.

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
- Emebbed the algorithm into your system so the user can just type a word or sentence, the system generates the safe password, which is used as the actual password.

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
20 - _iO$4=t7r/gV&|l0]UI{

$ ./test_avalanche 61 popeye "my norse horse" "I love mami"
61 - _iO$4=t7r/gV&|l0]UI{^@)mh#w8BNp+{)8c(!Dx/Rdm08[|5d@KmBt|-J@yS
61 - &G5@$6~yALN#=[&!wI0KoUx_xV|d(#3=1#]W!z|G)$#/p(!V!IV{P5aRL$^Z#
61 - =fE^0erLvb@4mKRY^D@7kVtXvSd!rI[1CkF!#xDUgIcHb4gO7bn7!wS7DrIO^

$ ./test_avalanche

**** Test of pa55w0rd_avalanche() ****

pa55w0rd_avalanche(            a) = 35 - K3xG@u@fTN^nU+(^}@NbnZmsP-EA}uRU)BeR
pa55w0rd_avalanche(   atmosphere) = 35 - !m5#8Z{ZcoA_/_xB/nvpBYs/Q^FZFxXNh#Z}
pa55w0rd_avalanche(   basketball) = 35 - y/XDQoG6mPn!k!N9z@/h({UjwxM-cAf[H[{V
pa55w0rd_avalanche(         bury) = 35 - X)(E{xIU$OiNQL8[~cA#^lA7#VS|/KIJ$18G
pa55w0rd_avalanche(concentration) = 35 - c2lZ=$O|u$-#_CZO2}p~}Fv#]FrrXT(JG+x@
pa55w0rd_avalanche(         copy) = 35 - imQP$VN0YgJ!V]]E5JCmiI}oy!xUlE@C4SIo
pa55w0rd_avalanche(          dog) = 35 - ^5ejRJxAW#^ASMbb!rXB@|QGwZ6x$VwOB)[_
pa55w0rd_avalanche( dramatically) = 35 - xp-s4nk!k[g!}u{xwaD=xwQB2Sxi{lRMXgh#
pa55w0rd_avalanche(establishment) = 35 - -~R}i=/D$k{|a0i8[SAM2m71i-!ld!U#G6si
pa55w0rd_avalanche(         ever) = 35 - mTV!QsH}adVE1jI=nk)vo)0j8-2~@@#hEzS}
pa55w0rd_avalanche(  frustration) = 35 - cmMr$/ob!YNo-B4-&L^~AeQKG@G!ZxNO9ONw
pa55w0rd_avalanche(         fund) = 35 - h^X^{r|C6Co/M@h#2d8/cHF3j]0YPbPfi[RM
pa55w0rd_avalanche(  grandfather) = 35 - 5BVh~|Z93ud$Gro]SvWQ=BN!herEs7$^CO)1
pa55w0rd_avalanche(           go) = 35 - hnW8m&Y3aONhBX4pF]l7xrORK+&D8JZ4cv{&
pa55w0rd_avalanche(           hi) = 35 - p@S&0cEcApdYXsA!BYRKBV]JYx2+G$]^5oTD
pa55w0rd_avalanche(   hypothesis) = 35 - ZaN/fhr9^8WW$TvvN)Dit!|e]Aj@v@b5r&!i
pa55w0rd_avalanche(            I) = 35 - Zjc[31EMxm^o/aDnMkdbDUg#lH|b!4#_Z!s(
pa55w0rd_avalanche(  involvement) = 35 - /Ts4AB[$X6=_B]_Rt2Y!Y91$W~BYhCJ{O_2G
pa55w0rd_avalanche(         joke) = 35 - ^xxnm1W!7G7o^-cgNo#e=Ymm1nT5#Hh6xx(c
pa55w0rd_avalanche(   journalist) = 35 - x/LjkFX^VIp7go!@1#Z/HkIIw3G3A84He$Vc
pa55w0rd_avalanche(         keep) = 35 - nkUwfkuKj$nF2g3!G(J_08$}43[OD@OiR!9-
pa55w0rd_avalanche(     kangaroo) = 35 - Wn@(D)@@][SY@^#b~&@!/gZ#!@I6$fQxKLnw
pa55w0rd_avalanche(   literature) = 35 - lEZ!xw~|6lh{)u^5D@!!R#D4!KM[zVScy@9Z
pa55w0rd_avalanche(          low) = 35 - UfXm}9O^aIw3N#jnpX5rx!NMMbt#wf=Y$Ph9
pa55w0rd_avalanche(         moon) = 35 - ^[c91omV!^{{5&Zn/+Wy7wZBie(f~#/{G4}W
pa55w0rd_avalanche(   motivation) = 35 - @vX^5h@=KwGX[#2x_2c0az8d@[H(54p7W)S4
pa55w0rd_avalanche(         near) = 35 - F&Ci!n^1-HHO!kkD3m0pWF9fnD9!VhNhbfC0
pa55w0rd_avalanche(  negotiation) = 35 - F[x|!!^(!(Fd7m2feBIXBmI#B}F@fyes6R@J
pa55w0rd_avalanche(    otherwise) = 35 - 4SCC/!Omx+ZbBh1{@Iy57np-TFuKw$@7d$Sy
pa55w0rd_avalanche(          own) = 35 - }ap3/ANxuYnTh4~@M!Z!8id/!l^_9!16)r45
pa55w0rd_avalanche(   punishment) = 35 - VWRQ2!#mr{E{fyA#~UAfm4!RQ)=tL@M//l}B
pa55w0rd_avalanche(          put) = 35 - 5[|C8yEvuzMW)GMu7e5QW!Qu&J~k8@0!1@ZZ
pa55w0rd_avalanche(  quarterback) = 35 - jVo&#M]4!a~xN^3#!VZaza!1{j427pjPr(#^
pa55w0rd_avalanche(         quit) = 35 - iCK_jI(zUljO#VMN7bS^#4y+O2o$(Uh2xl{3
pa55w0rd_avalanche(  responsible) = 35 - t+5g7c5rtNdhZNPDE_ci^@5T^!HU[Hx@99Yg
pa55w0rd_avalanche(         rise) = 35 - @gPcmwj26$X@@@/}L2_3t_/A+p_Bx_z=G!#g
pa55w0rd_avalanche( specifically) = 35 - |PmKS_((K@@P_@1]{7X[N$Hk^(As4x@sV3A+
pa55w0rd_avalanche(         spot) = 35 - X5PI$P@iw3XCZ=8BbugA^!dnR[@}a$1odBuM
pa55w0rd_avalanche(   tremendous) = 35 - An@{@vzuUX(+a)[{/0Lk-7etMn!@S&-dTDc!
pa55w0rd_avalanche(        trust) = 35 - oI!hWX@7|gi~t5}f@$gb3L)@QA^JjyUPLAU!
pa55w0rd_avalanche(         ugly) = 35 - RpiJ$V)y/5Xx#^bewAim{xfQ$}fVl(Bbg|$h
pa55w0rd_avalanche(      utility) = 35 - Mf@iMMQEk|!Q&7Py6&HVPvjAHze!AiX5~@(5
pa55w0rd_avalanche(         vast) = 35 - Wv&7^WCgjwR[ox]@$BUDD|AL!Xlm_CYDQ&^h
pa55w0rd_avalanche(      visible) = 35 - b!Ted{n-ncx!-Dn9QEi8xYHFH7L)t+O)5W#]
pa55w0rd_avalanche(     whatever) = 35 - Z~Vw9i#UVeM5@nL/L_nS!h_+2RKj##x)61Wa
pa55w0rd_avalanche(       window) = 35 - 4jU6@i[#3@3^[90{!CgS=TO6VexwO@CRA!Gn
pa55w0rd_avalanche(         yard) = 35 - @a!4OEvr#W-8@xw+K|-~+8~b$tcUZjK{DI9E
pa55w0rd_avalanche(     yourself) = 35 - my+_MaN1[Xu+SPo@-M3uok]Z=ToXYsUp4FpK
pa55w0rd_avalanche(         zulu) = 35 - D#px@GV!fLd2fW3bw_!@KjEJlzK)!=n+YIBL
pa55w0rd_avalanche(      zachary) = 35 - fr@9NnL1w{xULgY^V{9k|s3{OLQ!3SUmu4^G

biombo:avalanche guille$ ./test_avalanche 165

**** Test of pa55w0rd_avalanche() ****

pa55w0rd_avalanche(            a) = 165 - K3xG@u@fTN^nU+(^}@NbnZmsP-EA}uRU)BeRWK{dz)Go8dclR|L]Z-Rc#_PDa8+b!V[!}VU+TlFQk-]W!p)k6_R20[!G~0a!10Ji&kTQ@EtrY41|X4r~~)Z@(!dLZRel_ahefCKF)nv$KWNGru1DWWcG8Ke)+Ctz@({vj6
pa55w0rd_avalanche(   atmosphere) = 165 - !m5#8Z{ZcoA_/_xB/nvpBYs/Q^FZFxXNh#Z}x+hnj}bO(4lF!A!#@--wN$5-C@FIc/kX#k!feHm#C^3g}FC|@I7RYWRU$9ymmBFST@-3f7=|@N8^i!QAcA3i-$a-V6M(&_PfL2xbN|H@2woEhu~!09GY8lxM@R0$[^[!DO
pa55w0rd_avalanche(   basketball) = 165 - y/XDQoG6mPn!k!N9z@/h({UjwxM-cAf[H[{V@NfUi@rSL-Nh[vE_13Gm1bDHZ]42)_(TMdp@WSt[f#S6zwSEL-2dnN!p+!3l2Hr{GwPxZ)[xpSy}N^C#fHSQ4okga2Ro9M(B-^^#8@#F[MXY#Uy8tkFHJFjHlu|D9Jzs-r
pa55w0rd_avalanche(         bury) = 165 - X)(E{xIU$OiNQL8[~cA#^lA7#VS|/KIJ$18G@J(Mw2BT!4$~~$dR9_)U3ldp)-]LeYA[{KxIj!ZGK!)^{=XTw^Rp-[i^YGY_Ke@@0+DU=Q}xhJ&1Tm(dY-4~Ks@_fR-_0Tjkj+Mzm@=Z!)_$|ML[[egse2{c_!0r7N&bBN
pa55w0rd_avalanche(concentration) = 165 - c2lZ=$O|u$-#_CZO2}p~}Fv#]FrrXT(JG+x@#xwC}a]=RB^jE(!vSl+FWd!m1@}xBL6fOSJfQeop(Jo(bV/W7^gZ5jmbl4(Rys{EYv+89)U$r!&gc!4KL!geGXfi-@$/0@&(DjPYykK95c!GE@_02X~4U~LdaCT_BGRwaj
pa55w0rd_avalanche(         copy) = 165 - imQP$VN0YgJ!V]]E5JCmiI}oy!xUlE@C4SIocLzXUgN}7![Rrm3d&T@-]4}]@@]Gpwghl+#A|O)jlXXlA/RJ9T)Z!rwA$2#B9ue[B!9FXbnPe!1(+/w$w#InZ/Xp$@^_]y6ElxNQCx{uOPVGSC0(-a$PWVlER^wK/@(oNE
pa55w0rd_avalanche(          dog) = 165 - ^5ejRJxAW#^ASMbb!rXB@|QGwZ6x$VwOB)[_@[^_m3#66{c2^VZ{JPuow#Ry_I)[ZmWGnh6$5FdxU6jPZ6#[9~aPrYt$8!4NO$zN&j=KlSl#h#3grN/#ypQaF!Q!)!7vjw0rodrXf+JET!SF_D@v|/mxo~k$}AOd#-e~#[
pa55w0rd_avalanche( dramatically) = 165 - xp-s4nk!k[g!}u{xwaD=xwQB2Sxi{lRMXgh#W7HKRhv|QT&ZD3j$cl@kQKcBRE]!/5#ueN3C]AQwDo$/U8V_4fXI0NO$p!A#L$J6mgD!TX5wtbCcEO@/fQX]6-@#k@N#iWtX!!G+Ir55JWRdjafp^CX/wkY9!=xe|x&g{3
pa55w0rd_avalanche(establishment) = 165 - -~R}i=/D$k{|a0i8[SAM2m71i-!ld!U#G6sixWQi1@&N9^A5!/(cP6Uf()U{W6Ybl@VHMrk3j!m6d@(NJ!oCrrN]hW@kSxXbPrP^i6AKCxna1(jj^X7z=l6)!(d$($(&4!Zi273!jS^!AtNdZ$]uMQBc4G!7xPd!!|)YuW
pa55w0rd_avalanche(         ever) = 165 - mTV!QsH}adVE1jI=nk)vo)0j8-2~@@#hEzS}$JTJxWOJ#kY]0/F2U2=I0#Ub61)ZUOLkx7/Qi_TIku!Xw_w){x-FEJH@^!p!B#@-1ooW}WwWFJ!T={#N!Zb$ScBLb1)-d&3fAN&!v+}pmM$pcB|C-5^i|cp1^[WyJ@]!Uc
pa55w0rd_avalanche(  frustration) = 165 - cmMr$/ob!YNo-B4-&L^~AeQKG@G!ZxNO9ONw0{FMco2/Z#W3^K|ZnzJ@s/Diy7nT|GuE~1iel@/Umgyxr^x-+&J9x|!=3#6YB7U=yJl1kSJMcc-u#CYzH$Rw-4Q8N@T]}!W)Z$ouli+R2{P^A=LE|Fu|3F/+P#FT6c1tN#
pa55w0rd_avalanche(         fund) = 165 - h^X^{r|C6Co/M@h#2d8/cHF3j]0YPbPfi[RMLA^a|(@k6{QY~~#(uw@@a@peMC_+dLJDQM+n(P!=oReRAQ-9##x&2]$~&xY@6a2y}-sf!y$0Ae!ZbTRwAWbOwfXw[@^Fp{@+EG!d^+|8z1P_TTuxOrSNJZ49_B^g^3U_[7
pa55w0rd_avalanche(  grandfather) = 165 - 5BVh~|Z93ud$Gro]SvWQ=BN!herEs7$^CO)1=hT8@~~]cJ7S-LM3paXv^kkoFYFbdp9o)0@6P^ePxcFDM7A^0$8sy#87UR6y|B71^Flg]MT|NxxD^a63aV$w$&oVnnk$LmPbd@!RCMaG#RhAs3R~fVB30H9!mp2#w6I9!m
pa55w0rd_avalanche(           go) = 165 - hnW8m&Y3aONhBX4pF]l7xrORK+&D8JZ4cv{&AGVY|YZZ!(&f#j3/+Yv1aJ@Nww^48+v^yl~@37x|Wm[+_tYmjrPx~(]&m{@w6@HWb(HfKGvZ8n!elbZ$!9^mcip^i62k]6y~~tNa$jl@JD5Xj_XPviPuaI+l@bX5lK]#!2
pa55w0rd_avalanche(           hi) = 165 - p@S&0cEcApdYXsA!BYRKBV]JYx2+G$]^5oTD!)OrpMFCm~sNmpevx_I-k|OBaE=j-IEp^-~(+=/R&z+2@UCi~]=V19_!mVcM4!e!I/L^/+8x2J#L$N$|5e1sR=a{O22^$wMDxQOvSVlKyURj67Tf4dXa^ES1$64K3un1~5
pa55w0rd_avalanche(   hypothesis) = 165 - ZaN/fhr9^8WW$TvvN)Dit!|e]Aj@v@b5r&!iXfTn83iD[6eOP2/8M)^hus[pZpnlFD[j^fG5eR!$92PxK72/o!Q#|#p}j)rc@I$&$3z]wh^bL/!]$^x)4-w@0HdK=zYfD2kM!!!+lYdoKnb^})@C6H(Pxnn/ZQX{jrxEa_
pa55w0rd_avalanche(            I) = 165 - Zjc[31EMxm^o/aDnMkdbDUg#lH|b!4#_Z!s(Dxm&gWkK8=upt1$-m8QDLHT@ExO1orc#DWm|8SX7TVP[uuZmH0THB7jQCA3OgR{Z|a!@E9)AOHKdXy9!t2+^@4(P_}!#+p&xV}Gw_~oJxHi1mufIPk&TFD)c7gwz5Vb9-N
pa55w0rd_avalanche(  involvement) = 165 - /Ts4AB[$X6=_B]_Rt2Y!Y91$W~BYhCJ{O_2G$6gRPi!EEpb&a)h)lXEKJf|W#zeWfdBZ^C@[^bk/^_(Nw@!j&wOWe@9dNmDA(JVxWA@ZB~H^d9FexUxk}W@U-w~^!{DYL]ZCx$Z~J(V!l@5V+Vma$=Sd#U/70lMIFM@^EJ
pa55w0rd_avalanche(         joke) = 165 - ^xxnm1W!7G7o^-cgNo#e=Ymm1nT5#Hh6xx(c2J[)]7$1@xH)dTin@Z-k7NQ!CZcXBG6MF(p!9wn@eR54Q&w-/+GRTMdwd@YJf{aB/!7B8Ox$6c)z-1!IS4dwZweLH$@n{iWZlI^}pWSnx|+@w+V@dZZvDT@3L6ze+3x2!=
pa55w0rd_avalanche(   journalist) = 165 - x/LjkFX^VIp7go!@1#Z/HkIIw3G3A84He$Vc1V_^Px(^!ySEQc!vD&NP)y!$4Z@U8sG$B@98{p]!+&cxa4JM)}jrmrjX-g-MS!Ml3sl}UsHzQ@N&ie^VD}m+E$a(NC/NWriUDp)Ww+=Rs@vE&TZjH{^=xY^rl3MDYa}Q!]
pa55w0rd_avalanche(         keep) = 165 - nkUwfkuKj$nF2g3!G(J_08$}43[OD@OiR!9-n5w5|Fg!G~L=LSE~U|8^7vN{!egZk_W-RDDVavYj14C(]b!-#f]rQ+^prv#x$Vn!yfSMkAN~@10Kg~9IuiUCm#E!QamQD@@@/~docrX@x3##$VwlVXH@K$X)nRuephhoZj
pa55w0rd_avalanche(     kangaroo) = 165 - Wn@(D)@@][SY@^#b~&@!/gZ#!@I6$fQxKLnwl@&k_aPyjHo_ahGmBF-+Mu-3$8Zxl^!{d6G@1)]|yKYx}0Hkd@D9{Zt!^CK=4RX!Saiu$5W]{^_l[)@S1|W18Q|GA^(9DEcd=~&7@PAfLC[xR+R@-(RMTYbZm+tb+@K]]^
pa55w0rd_avalanche(   literature) = 165 - lEZ!xw~|6lh{)u^5D@!!R#D4!KM[zVScy@9Zzw5a5DJoPW^H7}5=x(YhA6!ORLlRNv)bb!PPmK}j[25cLaiW-]JV3WL2VpeZkRD+@!n+CzeO1@TwG^!IN#kT@k]E#ut$+I[$GxMKD@#+y/#c/fvi8u!3|FgJU1-SAI#WuK
pa55w0rd_avalanche(          low) = 165 - UfXm}9O^aIw3N#jnpX5rx!NMMbt#wf=Y$Ph9haeAg#7x6t/xj4L!}mwJ)6UfTft[m!c$W&g0G9INTzAnj+2#P^8Z[M|3RSvkrnT/{AIKBrkr5OuE!@x_)3ibYcXm4@|OnEgD{fx$!$8i{J_iBhllP==PfCgvRXK9^!Kv|L
pa55w0rd_avalanche(         moon) = 165 - ^[c91omV!^{{5&Zn/+Wy7wZBie(f~#/{G4}W^!H=m3_rV=RS37FRh@eSe&ZMrF{x&p[Q-p$@8k-(|EI~Y#S#@URLmYk}DiOs7pa=uPGMQf6NA_$$$MDlP^}J7zC]F^NgU&jPP!p|o){U4!VMh1U$rhH(5j|_onuex5ne9!
pa55w0rd_avalanche(   motivation) = 165 - @vX^5h@=KwGX[#2x_2c0az8d@[H(54p7W)S4Cp&{6laN!&B!Ra@cD2AF&bor=6hno4xA$2J@Lv!)!Kd2/{N^(bENN~Ypl@UnmT#Vd1bl430@jM}wyZ_j/wCm5hxDj@#WlL2fik=G/A~H!mQ48gJ!^$QD+=vl49[WUNZeVr
pa55w0rd_avalanche(         near) = 165 - F&Ci!n^1-HHO!kkD3m0pWF9fnD9!VhNhbfC0B!#s+^]kd#MG!ED(ma_x)h7$aPK4!r{a@|&Qwzc!RKa9O2De)cS!!FMZe)h2fPLx#({0_D$Cf!up}ijWrDG!!^_G_H6nl=8_C~c^JPr3a8PkKc7oA6e@&S(CR]0x(n5$!B
pa55w0rd_avalanche(  negotiation) = 165 - F[x|!!^(!(Fd7m2feBIXBmI#B}F@fyes6R@Jo/HkAff@]W9({n!euzo+piH)!!am3&h()4EcjCs^l$/=Xm[$Wn!3DAJ)@d!$gYCt^bcp5$F}=G2Tz8HBK0o#|7!zcgf#pr^|vC39YpuLoR]tU^JOKP+!{QU^89fH[@#FeZ
pa55w0rd_avalanche(    otherwise) = 165 - 4SCC/!Omx+ZbBh1{@Iy57np-TFuKw$@7d$Sy(cI@Ixe3y^jvk6asxws6jd!ULgPnyZ#t&OkJb@+wTnVD9LK=#QKV1]V)hxFK9^LCDRwH2a$ae@4--/iBWwTje^H2eDX9!E}u2BYnuS_!ci)|WBY(F{)3M8Ar|zWRK!g@uR
pa55w0rd_avalanche(          own) = 165 - }ap3/ANxuYnTh4~@M!Z!8id/!l^_9!16)r45cj[WTQ|ON!n5_@g[SMl+F!CO$o+l#FmXcG@4JGFEk{V4kJ0ss0&rPo@3#Y+U/Vhnzz##/4OX0-egk$iVAG)^N(Z1$F+LHpy=9B!QJm(JiB]!3lI5p/x$gw/W#0=!@!on=]
pa55w0rd_avalanche(   punishment) = 165 - VWRQ2!#mr{E{fyA#~UAfm4!RQ)=tL@M//l}B!iI/7KF^$^TP3B3IF^|V]x7@-hn^{}bD@GwM@~/$5lh&IMxK)Cfc!^PgA!UbOH4BPJJmMtDPC!rh#f{SEQFSV9^BXKmR8~i$D$}6{B@-XOwSuIHB9l}XwXEkn@b1^FKLWw
pa55w0rd_avalanche(          put) = 165 - 5[|C8yEvuzMW)GMu7e5QW!Qu&J~k8@0!1@ZZ!5DNygiYkiwh6xcBI/IT!Ek@vLK_iXS)22PTKZaa1l#VJrZk!58H3!BfciKX!gUwc^0!T)H8tjL8TSO7xX/4([s4!e)!H^BFm0Bv~Wufn}N}/c#_pX&Zx##jTV#+I@P19_
pa55w0rd_avalanche(  quarterback) = 165 - jVo&#M]4!a~xN^3#!VZaza!1{j427pjPr(#^Xw#3(=^^lh3$t3&UK&=Y=_t#b9cn{]$3xM54vGiCt{ko7aRhy~2!A/Y+FTw+{@^nBbNxX|mD^l!X5/l=_#=0w5!LlLo@7ob]Hx$!9klW[^|U~(#mb41)16MK8CWYX@P@Aw
pa55w0rd_avalanche(         quit) = 165 - iCK_jI(zUljO#VMN7bS^#4y+O2o$(Uh2xl{3cGjdnSw!tVhhFQt97KPZum!&YiE~Vxv$c5UxCAk~XaAc^D|x|^xmae&+wk2)B9@QLjj!mu3mg[!j}TpTwn@X2xkF8jldX6g_eYBOu@wR!-$zF}z4//BC!FBG^DOn!SA^pU
pa55w0rd_avalanche(  responsible) = 165 - t+5g7c5rtNdhZNPDE_ci^@5T^!HU[Hx@99Ygt_(8WEm/!(_9!09a+IJ6O$j=M##tVZZmIkkr@N@YVFsbcTk9]k]]3r_)=p{V-=G9IQV4[jKx@Pb|N1DaHWPdNhNm6HX|wl@@pL[om+9kh0o]9iDgA4U!NKvbdeakEA{|Fx
pa55w0rd_avalanche(         rise) = 165 - @gPcmwj26$X@@@/}L2_3t_/A+p_Bx_z=G!#ga2b8YjYX!6C-jau~#~L_93B!(@ZVT8(B&0MI(G!CYCo&#^]raf^nn~+3fa=[lcl3(!a[g@eoTD5gdX$#w$](uY=5$w98{_93$)^)12b#v_N9Q&$rYrDpHNCR^VP17Fs&r^
pa55w0rd_avalanche( specifically) = 165 - |PmKS_((K@@P_@1]{7X[N$Hk^(As4x@sV3A+=O/vWvE@4nV^L^+vr4Yc(=OZNE+$)2^hKBI{Z8ixK{Y#BP)psef^0ici@[9^nLL!W@++8sOz+_m6z-_TrWYup#}$rwR/zI@~!HfB!Ec8Im)9$)IKv#effH#B|reB@NpYo(
pa55w0rd_avalanche(         spot) = 165 - X5PI$P@iw3XCZ=8BbugA^!dnR[@}a$1odBuMp-YGAsp#3^^hlBp]JWwL!A[!y9aAY~Uwg@#pZNXYw1AWZ#X5^@099IYIK+[#lS3+Y$w$@ct80MuilG-R!BerJ!s@Koe~@[~_+G|$14wT/w3KDW-0V7!{~p$^S]]-D)d8d!
pa55w0rd_avalanche(   tremendous) = 165 - An@{@vzuUX(+a)[{/0Lk-7etMn!@S&-dTDc!!whxn7+!ez^/LNY3-01Pw0&YE]_nxSyexiDLl2mF!B4Wy~I_lMb)8R8)~OTJ!wm-LJ=b)b8zxL!_Bw@&b]}rs3{[F^lFlBZ9^=C}$^M0x+a9e-Qp]QCYCIMy-h#j17)IgI
pa55w0rd_avalanche(        trust) = 165 - oI!hWX@7|gi~t5}f@$gb3L)@QA^JjyUPLAU!YD|!=@!91-d@oy6A{TAlRI8N5HtH()@ghf!^3xmn^1nT!{iJfv2]Ou335Jk#5#S{@uYg-4E|bC^M@U!^}y8x4QQ#U4Q#St+#)I9wIxNj[-=|YhE/~m#L{+!e(ySdONRd{z
pa55w0rd_avalanche(         ugly) = 165 - RpiJ$V)y/5Xx#^bewAim{xfQ$}fVl(Bbg|$h$dz#P&Wj~Q[n{pWPFGjU|w7|V9~Y^8N_RMo]]Xx2Atu[lF{v4dTg(KiTAg#1UF+P8]#{V)r5|Akdox7}LO!o/##Qe7J5k!UPK#/[2}Q#O1knBK]7@-G5$]I!cjUB98We4f
pa55w0rd_avalanche(      utility) = 165 - Mf@iMMQEk|!Q&7Py6&HVPvjAHze!AiX5~@(5$psPM$3(QE@aE6JA/=@pJXf!$+w6[42xp7$z{3vJP9e_@z1x#_(Rg/xQH8=D5-atmKf-B1HOEbw+)Z[5y4i|PiwXr_L@~BKp7u9a|g)@G@!)=]lcJ9_#!-[iel-X!2}#&V
pa55w0rd_avalanche(         vast) = 165 - Wv&7^WCgjwR[ox]@$BUDD|AL!Xlm_CYDQ&^h@!6M5FJ#NAai@$h-C@xe[|6KwQ&Ql{@T8#sT1!QBB/en~sj~-GKAD$WY$cc1J@Vgh#yVx/^pCr&^b9Yd4LLksmegQ|}!|&W5Qf(~K)9]9Y8fos}7+SUin!6XdQw{KJ~@F@
pa55w0rd_avalanche(      visible) = 165 - b!Ted{n-ncx!-Dn9QEi8xYHFH7L)t+O)5W#]|@wTM[L35!$wKX32HD]4ACObj[-Br9nCPh4oXbDDZ=f^@$T~X~x[yUfT8lI}UvKSZTDN13e}1xNdsnSz6Oo~w5pMdg#~MRYW!y46plUkbBS+XPcd|kp5~78YC/kStstwSX
pa55w0rd_avalanche(     whatever) = 165 - Z~Vw9i#UVeM5@nL/L_nS!h_+2RKj##x)61WafZgT9EoR4MBc&@A588n}b&/^)D0xIVw+_}OkrbeWw)}Y1g#6QwPzYDm^$y&[c/g3CoDFGufvOJlGDSY68)_KJ=$5j!ke@b#wiO!lci0m@c^XJAx3br=v$}ts9^/n#FPi6/
pa55w0rd_avalanche(       window) = 165 - 4jU6@i[#3@3^[90{!CgS=TO6VexwO@CRA!Gn=@ddi&}uQwUb8/k4@ifdb8~9!1^lw-}-G+-UBx@]@3|-h&3S#@M&@x{AQjw^@ic^6X(3/A^unQz8F[G-lK$!eUSMO22P#]x0Aoj$aE(3DDOJs(+P7(=N#BuLQ!/GTho7|E
pa55w0rd_avalanche(         yard) = 165 - @a!4OEvr#W-8@xw+K|-~+8~b$tcUZjK{DI9ETR@~JV|$aXZ$]V/Tu1(^UVarmnL@NU@PXlLlc2Fu2r[^VZil+TdkwJHS&]CS!4s+y}d!z#EMC}@+1g)+)K=ZOOB#L^)b+J905zT5=eGx1@/9(nlJ)K@S1_G-_#9Q8!DVh5
pa55w0rd_avalanche(     yourself) = 165 - my+_MaN1[Xu+SPo@-M3uok]Z=ToXYsUp4FpK1=!|(3Ea_K!H}1gS4Df|W#ldK=V)e#rJ^#!Ihe!uV{C_}|xr!xW)R6XLU~(R2$V^I/S3{|l7oW7!$Y-c4g241bkwF!9j]nW-6Cw)L4lOii2SWo(LvLARy$Vnu3cs=w!zSd
pa55w0rd_avalanche(         zulu) = 165 - D#px@GV!fLd2fW3bw_!@KjEJlzK)!=n+YIBLQWpx7$PF!pDSfA$At!~~a[K@@n[d!C@]F^=X@epg=@G^DieiNe!dckHe~6!^l_!Y+!4^z-QH-$A@FL}#1xPho@Q4=R_1Cwjx#D=R7Og!p{~&mQiQ+~#p6Se72kGnIXMDX[
pa55w0rd_avalanche(      zachary) = 165 - fr@9NnL1w{xULgY^V{9k|s3{OLQ!3SUmu4^G]wjUi[t!7U+pLeMk!GnKJ9QVdX9!4l7#w)BY6#ZRUP!Z(Yt~Qj{4SlpI8cZVX|E!rK@^2--IG~41LQ9d6]8WB2t7=v3wr&xX2pgJVoW|$10-{ZuXsbgA5!&j&_z1wO[c)G
                    
```




