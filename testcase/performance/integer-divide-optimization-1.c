// gcc integer-divide-optimization.c sylib.c -include sylib.h -Wall
// -Wno-unused-result -o binary-integer-divide-optimization && time
// ./binary-integer-divide-optimization < integer-divide-optimization.in
int loopCount = 0;
int multi = 2;
int size = 1000;

int func(int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8, int i9,
         int i10, int i11, int i12, int i13, int i14, int i15, int i16, int i17,
         int i18, int i19, int i20, int i21, int i22, int i23, int i24, int i25,
         int i26, int i27, int i28, int i29, int i30, int i31, int i32, int i33,
         int i34, int i35, int i36, int i37, int i38, int i39, int i40, int i41,
         int i42, int i43, int i44, int i45, int i46, int i47, int i48, int i49,
         int i50, int i51, int i52, int i53, int i54, int i55, int i56, int i57,
         int i58, int i59, int i60, int i61, int i62, int i63, int i64, int i65,
         int i66, int i67, int i68, int i69, int i70, int i71, int i72, int i73,
         int i74, int i75, int i76, int i77, int i78, int i79, int i80, int i81,
         int i82, int i83, int i84, int i85, int i86, int i87, int i88, int i89,
         int i90, int i91, int i92, int i93, int i94, int i95, int i96, int i97,
         int i98, int i99, int i100, int i101, int i102, int i103, int i104,
         int i105, int i106, int i107, int i108, int i109, int i110, int i111,
         int i112, int i113, int i114, int i115, int i116, int i117, int i118,
         int i119, int i120, int i121, int i122, int i123, int i124, int i125,
         int i126, int i127, int i128, int i129, int i130, int i131, int i132,
         int i133, int i134, int i135, int i136, int i137, int i138, int i139,
         int i140, int i141, int i142, int i143, int i144, int i145, int i146,
         int i147, int i148, int i149, int i150, int i151, int i152, int i153,
         int i154, int i155, int i156, int i157, int i158, int i159, int i160,
         int i161, int i162, int i163, int i164, int i165, int i166, int i167,
         int i168, int i169, int i170, int i171, int i172, int i173, int i174,
         int i175, int i176, int i177, int i178, int i179, int i180, int i181,
         int i182, int i183, int i184, int i185, int i186, int i187, int i188,
         int i189, int i190, int i191, int i192, int i193, int i194, int i195,
         int i196, int i197, int i198, int i199, int i200, int i201, int i202,
         int i203, int i204, int i205, int i206, int i207, int i208, int i209,
         int i210, int i211, int i212, int i213, int i214, int i215, int i216,
         int i217, int i218, int i219, int i220, int i221, int i222, int i223,
         int i224, int i225, int i226, int i227, int i228, int i229, int i230,
         int i231, int i232, int i233, int i234, int i235, int i236, int i237,
         int i238, int i239, int i240, int i241, int i242, int i243, int i244,
         int i245, int i246, int i247, int i248, int i249, int i250, int i251,
         int i252, int i253, int i254, int i255, int i256, int i257, int i258,
         int i259, int i260, int i261, int i262, int i263, int i264, int i265,
         int i266, int i267, int i268, int i269, int i270, int i271, int i272,
         int i273, int i274, int i275, int i276, int i277, int i278, int i279,
         int i280, int i281, int i282, int i283, int i284, int i285, int i286,
         int i287, int i288, int i289, int i290, int i291, int i292, int i293,
         int i294, int i295, int i296, int i297, int i298, int i299, int i300,
         int i301, int i302, int i303, int i304, int i305, int i306, int i307,
         int i308, int i309, int i310, int i311, int i312, int i313, int i314,
         int i315, int i316, int i317, int i318, int i319, int i320, int i321,
         int i322, int i323, int i324, int i325, int i326, int i327, int i328,
         int i329, int i330, int i331, int i332, int i333, int i334, int i335,
         int i336, int i337, int i338, int i339, int i340, int i341, int i342,
         int i343, int i344, int i345, int i346, int i347, int i348, int i349,
         int i350, int i351, int i352, int i353, int i354, int i355, int i356,
         int i357, int i358, int i359, int i360, int i361, int i362, int i363,
         int i364, int i365, int i366, int i367, int i368, int i369, int i370,
         int i371, int i372, int i373, int i374, int i375, int i376, int i377,
         int i378, int i379, int i380, int i381, int i382, int i383, int i384,
         int i385, int i386, int i387, int i388, int i389, int i390, int i391,
         int i392, int i393, int i394, int i395, int i396, int i397, int i398,
         int i399, int i400, int i401, int i402, int i403, int i404, int i405,
         int i406, int i407, int i408, int i409, int i410, int i411, int i412,
         int i413, int i414, int i415, int i416, int i417, int i418, int i419,
         int i420, int i421, int i422, int i423, int i424, int i425, int i426,
         int i427, int i428, int i429, int i430, int i431, int i432, int i433,
         int i434, int i435, int i436, int i437, int i438, int i439, int i440,
         int i441, int i442, int i443, int i444, int i445, int i446, int i447,
         int i448, int i449, int i450, int i451, int i452, int i453, int i454,
         int i455, int i456, int i457, int i458, int i459, int i460, int i461,
         int i462, int i463, int i464, int i465, int i466, int i467, int i468,
         int i469, int i470, int i471, int i472, int i473, int i474, int i475,
         int i476, int i477, int i478, int i479, int i480, int i481, int i482,
         int i483, int i484, int i485, int i486, int i487, int i488, int i489,
         int i490, int i491, int i492, int i493, int i494, int i495, int i496,
         int i497, int i498, int i499, int i500, int i501, int i502, int i503,
         int i504, int i505, int i506, int i507, int i508, int i509, int i510,
         int i511, int i512, int i513, int i514, int i515, int i516, int i517,
         int i518, int i519, int i520, int i521, int i522, int i523, int i524,
         int i525, int i526, int i527, int i528, int i529, int i530, int i531,
         int i532, int i533, int i534, int i535, int i536, int i537, int i538,
         int i539, int i540, int i541, int i542, int i543, int i544, int i545,
         int i546, int i547, int i548, int i549, int i550, int i551, int i552,
         int i553, int i554, int i555, int i556, int i557, int i558, int i559,
         int i560, int i561, int i562, int i563, int i564, int i565, int i566,
         int i567, int i568, int i569, int i570, int i571, int i572, int i573,
         int i574, int i575, int i576, int i577, int i578, int i579, int i580,
         int i581, int i582, int i583, int i584, int i585, int i586, int i587,
         int i588, int i589, int i590, int i591, int i592, int i593, int i594,
         int i595, int i596, int i597, int i598, int i599, int i600, int i601,
         int i602, int i603, int i604, int i605, int i606, int i607, int i608,
         int i609, int i610, int i611, int i612, int i613, int i614, int i615,
         int i616, int i617, int i618, int i619, int i620, int i621, int i622,
         int i623, int i624, int i625, int i626, int i627, int i628, int i629,
         int i630, int i631, int i632, int i633, int i634, int i635, int i636,
         int i637, int i638, int i639, int i640, int i641, int i642, int i643,
         int i644, int i645, int i646, int i647, int i648, int i649, int i650,
         int i651, int i652, int i653, int i654, int i655, int i656, int i657,
         int i658, int i659, int i660, int i661, int i662, int i663, int i664,
         int i665, int i666, int i667, int i668, int i669, int i670, int i671,
         int i672, int i673, int i674, int i675, int i676, int i677, int i678,
         int i679, int i680, int i681, int i682, int i683, int i684, int i685,
         int i686, int i687, int i688, int i689, int i690, int i691, int i692,
         int i693, int i694, int i695, int i696, int i697, int i698, int i699,
         int i700, int i701, int i702, int i703, int i704, int i705, int i706,
         int i707, int i708, int i709, int i710, int i711, int i712, int i713,
         int i714, int i715, int i716, int i717, int i718, int i719, int i720,
         int i721, int i722, int i723, int i724, int i725, int i726, int i727,
         int i728, int i729, int i730, int i731, int i732, int i733, int i734,
         int i735, int i736, int i737, int i738, int i739, int i740, int i741,
         int i742, int i743, int i744, int i745, int i746, int i747, int i748,
         int i749, int i750, int i751, int i752, int i753, int i754, int i755,
         int i756, int i757, int i758, int i759, int i760, int i761, int i762,
         int i763, int i764, int i765, int i766, int i767, int i768, int i769,
         int i770, int i771, int i772, int i773, int i774, int i775, int i776,
         int i777, int i778, int i779, int i780, int i781, int i782, int i783,
         int i784, int i785, int i786, int i787, int i788, int i789, int i790,
         int i791, int i792, int i793, int i794, int i795, int i796, int i797,
         int i798, int i799, int i800, int i801, int i802, int i803, int i804,
         int i805, int i806, int i807, int i808, int i809, int i810, int i811,
         int i812, int i813, int i814, int i815, int i816, int i817, int i818,
         int i819, int i820, int i821, int i822, int i823, int i824, int i825,
         int i826, int i827, int i828, int i829, int i830, int i831, int i832,
         int i833, int i834, int i835, int i836, int i837, int i838, int i839,
         int i840, int i841, int i842, int i843, int i844, int i845, int i846,
         int i847, int i848, int i849, int i850, int i851, int i852, int i853,
         int i854, int i855, int i856, int i857, int i858, int i859, int i860,
         int i861, int i862, int i863, int i864, int i865, int i866, int i867,
         int i868, int i869, int i870, int i871, int i872, int i873, int i874,
         int i875, int i876, int i877, int i878, int i879, int i880, int i881,
         int i882, int i883, int i884, int i885, int i886, int i887, int i888,
         int i889, int i890, int i891, int i892, int i893, int i894, int i895,
         int i896, int i897, int i898, int i899, int i900, int i901, int i902,
         int i903, int i904, int i905, int i906, int i907, int i908, int i909,
         int i910, int i911, int i912, int i913, int i914, int i915, int i916,
         int i917, int i918, int i919, int i920, int i921, int i922, int i923,
         int i924, int i925, int i926, int i927, int i928, int i929, int i930,
         int i931, int i932, int i933, int i934, int i935, int i936, int i937,
         int i938, int i939, int i940, int i941, int i942, int i943, int i944,
         int i945, int i946, int i947, int i948, int i949, int i950, int i951,
         int i952, int i953, int i954, int i955, int i956, int i957, int i958,
         int i959, int i960, int i961, int i962, int i963, int i964, int i965,
         int i966, int i967, int i968, int i969, int i970, int i971, int i972,
         int i973, int i974, int i975, int i976, int i977, int i978, int i979,
         int i980, int i981, int i982, int i983, int i984, int i985, int i986,
         int i987, int i988, int i989, int i990, int i991, int i992, int i993,
         int i994, int i995, int i996, int i997, int i998, int i999,
         int i1000) {
  i1 = i1 / 2;
  i2 = i2 / 2;
  i3 = i3 / 2;
  i4 = i4 / 2;
  i5 = i5 / 2;
  i6 = i6 / 2;
  i7 = i7 / 2;
  i8 = i8 / 2;
  i9 = i9 / 2;
  i10 = i10 / 2;
  i11 = i11 / 2;
  i12 = i12 / 2;
  i13 = i13 / 2;
  i14 = i14 / 2;
  i15 = i15 / 2;
  i16 = i16 / 2;
  i17 = i17 / 2;
  i18 = i18 / 2;
  i19 = i19 / 2;
  i20 = i20 / 2;
  i21 = i21 / 2;
  i22 = i22 / 2;
  i23 = i23 / 2;
  i24 = i24 / 2;
  i25 = i25 / 2;
  i26 = i26 / 2;
  i27 = i27 / 2;
  i28 = i28 / 2;
  i29 = i29 / 2;
  i30 = i30 / 2;
  i31 = i31 / 2;
  i32 = i32 / 2;
  i33 = i33 / 2;
  i34 = i34 / 2;
  i35 = i35 / 2;
  i36 = i36 / 2;
  i37 = i37 / 2;
  i38 = i38 / 2;
  i39 = i39 / 2;
  i40 = i40 / 2;
  i41 = i41 / 2;
  i42 = i42 / 2;
  i43 = i43 / 2;
  i44 = i44 / 2;
  i45 = i45 / 2;
  i46 = i46 / 2;
  i47 = i47 / 2;
  i48 = i48 / 2;
  i49 = i49 / 2;
  i50 = i50 / 2;
  i51 = i51 / 2;
  i52 = i52 / 2;
  i53 = i53 / 2;
  i54 = i54 / 2;
  i55 = i55 / 2;
  i56 = i56 / 2;
  i57 = i57 / 2;
  i58 = i58 / 2;
  i59 = i59 / 2;
  i60 = i60 / 2;
  i61 = i61 / 2;
  i62 = i62 / 2;
  i63 = i63 / 2;
  i64 = i64 / 2;
  i65 = i65 / 2;
  i66 = i66 / 2;
  i67 = i67 / 2;
  i68 = i68 / 2;
  i69 = i69 / 2;
  i70 = i70 / 2;
  i71 = i71 / 2;
  i72 = i72 / 2;
  i73 = i73 / 2;
  i74 = i74 / 2;
  i75 = i75 / 2;
  i76 = i76 / 2;
  i77 = i77 / 2;
  i78 = i78 / 2;
  i79 = i79 / 2;
  i80 = i80 / 2;
  i81 = i81 / 2;
  i82 = i82 / 2;
  i83 = i83 / 2;
  i84 = i84 / 2;
  i85 = i85 / 2;
  i86 = i86 / 2;
  i87 = i87 / 2;
  i88 = i88 / 2;
  i89 = i89 / 2;
  i90 = i90 / 2;
  i91 = i91 / 2;
  i92 = i92 / 2;
  i93 = i93 / 2;
  i94 = i94 / 2;
  i95 = i95 / 2;
  i96 = i96 / 2;
  i97 = i97 / 2;
  i98 = i98 / 2;
  i99 = i99 / 2;
  i100 = i100 / 2;
  i101 = i101 / 2;
  i102 = i102 / 2;
  i103 = i103 / 2;
  i104 = i104 / 2;
  i105 = i105 / 2;
  i106 = i106 / 2;
  i107 = i107 / 2;
  i108 = i108 / 2;
  i109 = i109 / 2;
  i110 = i110 / 2;
  i111 = i111 / 2;
  i112 = i112 / 2;
  i113 = i113 / 2;
  i114 = i114 / 2;
  i115 = i115 / 2;
  i116 = i116 / 2;
  i117 = i117 / 2;
  i118 = i118 / 2;
  i119 = i119 / 2;
  i120 = i120 / 2;
  i121 = i121 / 2;
  i122 = i122 / 2;
  i123 = i123 / 2;
  i124 = i124 / 2;
  i125 = i125 / 2;
  i126 = i126 / 2;
  i127 = i127 / 2;
  i128 = i128 / 2;
  i129 = i129 / 2;
  i130 = i130 / 2;
  i131 = i131 / 2;
  i132 = i132 / 2;
  i133 = i133 / 2;
  i134 = i134 / 2;
  i135 = i135 / 2;
  i136 = i136 / 2;
  i137 = i137 / 2;
  i138 = i138 / 2;
  i139 = i139 / 2;
  i140 = i140 / 2;
  i141 = i141 / 2;
  i142 = i142 / 2;
  i143 = i143 / 2;
  i144 = i144 / 2;
  i145 = i145 / 2;
  i146 = i146 / 2;
  i147 = i147 / 2;
  i148 = i148 / 2;
  i149 = i149 / 2;
  i150 = i150 / 2;
  i151 = i151 / 2;
  i152 = i152 / 2;
  i153 = i153 / 2;
  i154 = i154 / 2;
  i155 = i155 / 2;
  i156 = i156 / 2;
  i157 = i157 / 2;
  i158 = i158 / 2;
  i159 = i159 / 2;
  i160 = i160 / 2;
  i161 = i161 / 2;
  i162 = i162 / 2;
  i163 = i163 / 2;
  i164 = i164 / 2;
  i165 = i165 / 2;
  i166 = i166 / 2;
  i167 = i167 / 2;
  i168 = i168 / 2;
  i169 = i169 / 2;
  i170 = i170 / 2;
  i171 = i171 / 2;
  i172 = i172 / 2;
  i173 = i173 / 2;
  i174 = i174 / 2;
  i175 = i175 / 2;
  i176 = i176 / 2;
  i177 = i177 / 2;
  i178 = i178 / 2;
  i179 = i179 / 2;
  i180 = i180 / 2;
  i181 = i181 / 2;
  i182 = i182 / 2;
  i183 = i183 / 2;
  i184 = i184 / 2;
  i185 = i185 / 2;
  i186 = i186 / 2;
  i187 = i187 / 2;
  i188 = i188 / 2;
  i189 = i189 / 2;
  i190 = i190 / 2;
  i191 = i191 / 2;
  i192 = i192 / 2;
  i193 = i193 / 2;
  i194 = i194 / 2;
  i195 = i195 / 2;
  i196 = i196 / 2;
  i197 = i197 / 2;
  i198 = i198 / 2;
  i199 = i199 / 2;
  i200 = i200 / 2;
  i201 = i201 / 2;
  i202 = i202 / 2;
  i203 = i203 / 2;
  i204 = i204 / 2;
  i205 = i205 / 2;
  i206 = i206 / 2;
  i207 = i207 / 2;
  i208 = i208 / 2;
  i209 = i209 / 2;
  i210 = i210 / 2;
  i211 = i211 / 2;
  i212 = i212 / 2;
  i213 = i213 / 2;
  i214 = i214 / 2;
  i215 = i215 / 2;
  i216 = i216 / 2;
  i217 = i217 / 2;
  i218 = i218 / 2;
  i219 = i219 / 2;
  i220 = i220 / 2;
  i221 = i221 / 2;
  i222 = i222 / 2;
  i223 = i223 / 2;
  i224 = i224 / 2;
  i225 = i225 / 2;
  i226 = i226 / 2;
  i227 = i227 / 2;
  i228 = i228 / 2;
  i229 = i229 / 2;
  i230 = i230 / 2;
  i231 = i231 / 2;
  i232 = i232 / 2;
  i233 = i233 / 2;
  i234 = i234 / 2;
  i235 = i235 / 2;
  i236 = i236 / 2;
  i237 = i237 / 2;
  i238 = i238 / 2;
  i239 = i239 / 2;
  i240 = i240 / 2;
  i241 = i241 / 2;
  i242 = i242 / 2;
  i243 = i243 / 2;
  i244 = i244 / 2;
  i245 = i245 / 2;
  i246 = i246 / 2;
  i247 = i247 / 2;
  i248 = i248 / 2;
  i249 = i249 / 2;
  i250 = i250 / 2;
  i251 = i251 / 2;
  i252 = i252 / 2;
  i253 = i253 / 2;
  i254 = i254 / 2;
  i255 = i255 / 2;
  i256 = i256 / 2;
  i257 = i257 / 2;
  i258 = i258 / 2;
  i259 = i259 / 2;
  i260 = i260 / 2;
  i261 = i261 / 2;
  i262 = i262 / 2;
  i263 = i263 / 2;
  i264 = i264 / 2;
  i265 = i265 / 2;
  i266 = i266 / 2;
  i267 = i267 / 2;
  i268 = i268 / 2;
  i269 = i269 / 2;
  i270 = i270 / 2;
  i271 = i271 / 2;
  i272 = i272 / 2;
  i273 = i273 / 2;
  i274 = i274 / 2;
  i275 = i275 / 2;
  i276 = i276 / 2;
  i277 = i277 / 2;
  i278 = i278 / 2;
  i279 = i279 / 2;
  i280 = i280 / 2;
  i281 = i281 / 2;
  i282 = i282 / 2;
  i283 = i283 / 2;
  i284 = i284 / 2;
  i285 = i285 / 2;
  i286 = i286 / 2;
  i287 = i287 / 2;
  i288 = i288 / 2;
  i289 = i289 / 2;
  i290 = i290 / 2;
  i291 = i291 / 2;
  i292 = i292 / 2;
  i293 = i293 / 2;
  i294 = i294 / 2;
  i295 = i295 / 2;
  i296 = i296 / 2;
  i297 = i297 / 2;
  i298 = i298 / 2;
  i299 = i299 / 2;
  i300 = i300 / 2;
  i301 = i301 / 2;
  i302 = i302 / 2;
  i303 = i303 / 2;
  i304 = i304 / 2;
  i305 = i305 / 2;
  i306 = i306 / 2;
  i307 = i307 / 2;
  i308 = i308 / 2;
  i309 = i309 / 2;
  i310 = i310 / 2;
  i311 = i311 / 2;
  i312 = i312 / 2;
  i313 = i313 / 2;
  i314 = i314 / 2;
  i315 = i315 / 2;
  i316 = i316 / 2;
  i317 = i317 / 2;
  i318 = i318 / 2;
  i319 = i319 / 2;
  i320 = i320 / 2;
  i321 = i321 / 2;
  i322 = i322 / 2;
  i323 = i323 / 2;
  i324 = i324 / 2;
  i325 = i325 / 2;
  i326 = i326 / 2;
  i327 = i327 / 2;
  i328 = i328 / 2;
  i329 = i329 / 2;
  i330 = i330 / 2;
  i331 = i331 / 2;
  i332 = i332 / 2;
  i333 = i333 / 2;
  i334 = i334 / 2;
  i335 = i335 / 2;
  i336 = i336 / 2;
  i337 = i337 / 2;
  i338 = i338 / 2;
  i339 = i339 / 2;
  i340 = i340 / 2;
  i341 = i341 / 2;
  i342 = i342 / 2;
  i343 = i343 / 2;
  i344 = i344 / 2;
  i345 = i345 / 2;
  i346 = i346 / 2;
  i347 = i347 / 2;
  i348 = i348 / 2;
  i349 = i349 / 2;
  i350 = i350 / 2;
  i351 = i351 / 2;
  i352 = i352 / 2;
  i353 = i353 / 2;
  i354 = i354 / 2;
  i355 = i355 / 2;
  i356 = i356 / 2;
  i357 = i357 / 2;
  i358 = i358 / 2;
  i359 = i359 / 2;
  i360 = i360 / 2;
  i361 = i361 / 2;
  i362 = i362 / 2;
  i363 = i363 / 2;
  i364 = i364 / 2;
  i365 = i365 / 2;
  i366 = i366 / 2;
  i367 = i367 / 2;
  i368 = i368 / 2;
  i369 = i369 / 2;
  i370 = i370 / 2;
  i371 = i371 / 2;
  i372 = i372 / 2;
  i373 = i373 / 2;
  i374 = i374 / 2;
  i375 = i375 / 2;
  i376 = i376 / 2;
  i377 = i377 / 2;
  i378 = i378 / 2;
  i379 = i379 / 2;
  i380 = i380 / 2;
  i381 = i381 / 2;
  i382 = i382 / 2;
  i383 = i383 / 2;
  i384 = i384 / 2;
  i385 = i385 / 2;
  i386 = i386 / 2;
  i387 = i387 / 2;
  i388 = i388 / 2;
  i389 = i389 / 2;
  i390 = i390 / 2;
  i391 = i391 / 2;
  i392 = i392 / 2;
  i393 = i393 / 2;
  i394 = i394 / 2;
  i395 = i395 / 2;
  i396 = i396 / 2;
  i397 = i397 / 2;
  i398 = i398 / 2;
  i399 = i399 / 2;
  i400 = i400 / 2;
  i401 = i401 / 2;
  i402 = i402 / 2;
  i403 = i403 / 2;
  i404 = i404 / 2;
  i405 = i405 / 2;
  i406 = i406 / 2;
  i407 = i407 / 2;
  i408 = i408 / 2;
  i409 = i409 / 2;
  i410 = i410 / 2;
  i411 = i411 / 2;
  i412 = i412 / 2;
  i413 = i413 / 2;
  i414 = i414 / 2;
  i415 = i415 / 2;
  i416 = i416 / 2;
  i417 = i417 / 2;
  i418 = i418 / 2;
  i419 = i419 / 2;
  i420 = i420 / 2;
  i421 = i421 / 2;
  i422 = i422 / 2;
  i423 = i423 / 2;
  i424 = i424 / 2;
  i425 = i425 / 2;
  i426 = i426 / 2;
  i427 = i427 / 2;
  i428 = i428 / 2;
  i429 = i429 / 2;
  i430 = i430 / 2;
  i431 = i431 / 2;
  i432 = i432 / 2;
  i433 = i433 / 2;
  i434 = i434 / 2;
  i435 = i435 / 2;
  i436 = i436 / 2;
  i437 = i437 / 2;
  i438 = i438 / 2;
  i439 = i439 / 2;
  i440 = i440 / 2;
  i441 = i441 / 2;
  i442 = i442 / 2;
  i443 = i443 / 2;
  i444 = i444 / 2;
  i445 = i445 / 2;
  i446 = i446 / 2;
  i447 = i447 / 2;
  i448 = i448 / 2;
  i449 = i449 / 2;
  i450 = i450 / 2;
  i451 = i451 / 2;
  i452 = i452 / 2;
  i453 = i453 / 2;
  i454 = i454 / 2;
  i455 = i455 / 2;
  i456 = i456 / 2;
  i457 = i457 / 2;
  i458 = i458 / 2;
  i459 = i459 / 2;
  i460 = i460 / 2;
  i461 = i461 / 2;
  i462 = i462 / 2;
  i463 = i463 / 2;
  i464 = i464 / 2;
  i465 = i465 / 2;
  i466 = i466 / 2;
  i467 = i467 / 2;
  i468 = i468 / 2;
  i469 = i469 / 2;
  i470 = i470 / 2;
  i471 = i471 / 2;
  i472 = i472 / 2;
  i473 = i473 / 2;
  i474 = i474 / 2;
  i475 = i475 / 2;
  i476 = i476 / 2;
  i477 = i477 / 2;
  i478 = i478 / 2;
  i479 = i479 / 2;
  i480 = i480 / 2;
  i481 = i481 / 2;
  i482 = i482 / 2;
  i483 = i483 / 2;
  i484 = i484 / 2;
  i485 = i485 / 2;
  i486 = i486 / 2;
  i487 = i487 / 2;
  i488 = i488 / 2;
  i489 = i489 / 2;
  i490 = i490 / 2;
  i491 = i491 / 2;
  i492 = i492 / 2;
  i493 = i493 / 2;
  i494 = i494 / 2;
  i495 = i495 / 2;
  i496 = i496 / 2;
  i497 = i497 / 2;
  i498 = i498 / 2;
  i499 = i499 / 2;
  i500 = i500 / 2;
  i501 = i501 / 2;
  i502 = i502 / 2;
  i503 = i503 / 2;
  i504 = i504 / 2;
  i505 = i505 / 2;
  i506 = i506 / 2;
  i507 = i507 / 2;
  i508 = i508 / 2;
  i509 = i509 / 2;
  i510 = i510 / 2;
  i511 = i511 / 2;
  i512 = i512 / 2;
  i513 = i513 / 2;
  i514 = i514 / 2;
  i515 = i515 / 2;
  i516 = i516 / 2;
  i517 = i517 / 2;
  i518 = i518 / 2;
  i519 = i519 / 2;
  i520 = i520 / 2;
  i521 = i521 / 2;
  i522 = i522 / 2;
  i523 = i523 / 2;
  i524 = i524 / 2;
  i525 = i525 / 2;
  i526 = i526 / 2;
  i527 = i527 / 2;
  i528 = i528 / 2;
  i529 = i529 / 2;
  i530 = i530 / 2;
  i531 = i531 / 2;
  i532 = i532 / 2;
  i533 = i533 / 2;
  i534 = i534 / 2;
  i535 = i535 / 2;
  i536 = i536 / 2;
  i537 = i537 / 2;
  i538 = i538 / 2;
  i539 = i539 / 2;
  i540 = i540 / 2;
  i541 = i541 / 2;
  i542 = i542 / 2;
  i543 = i543 / 2;
  i544 = i544 / 2;
  i545 = i545 / 2;
  i546 = i546 / 2;
  i547 = i547 / 2;
  i548 = i548 / 2;
  i549 = i549 / 2;
  i550 = i550 / 2;
  i551 = i551 / 2;
  i552 = i552 / 2;
  i553 = i553 / 2;
  i554 = i554 / 2;
  i555 = i555 / 2;
  i556 = i556 / 2;
  i557 = i557 / 2;
  i558 = i558 / 2;
  i559 = i559 / 2;
  i560 = i560 / 2;
  i561 = i561 / 2;
  i562 = i562 / 2;
  i563 = i563 / 2;
  i564 = i564 / 2;
  i565 = i565 / 2;
  i566 = i566 / 2;
  i567 = i567 / 2;
  i568 = i568 / 2;
  i569 = i569 / 2;
  i570 = i570 / 2;
  i571 = i571 / 2;
  i572 = i572 / 2;
  i573 = i573 / 2;
  i574 = i574 / 2;
  i575 = i575 / 2;
  i576 = i576 / 2;
  i577 = i577 / 2;
  i578 = i578 / 2;
  i579 = i579 / 2;
  i580 = i580 / 2;
  i581 = i581 / 2;
  i582 = i582 / 2;
  i583 = i583 / 2;
  i584 = i584 / 2;
  i585 = i585 / 2;
  i586 = i586 / 2;
  i587 = i587 / 2;
  i588 = i588 / 2;
  i589 = i589 / 2;
  i590 = i590 / 2;
  i591 = i591 / 2;
  i592 = i592 / 2;
  i593 = i593 / 2;
  i594 = i594 / 2;
  i595 = i595 / 2;
  i596 = i596 / 2;
  i597 = i597 / 2;
  i598 = i598 / 2;
  i599 = i599 / 2;
  i600 = i600 / 2;
  i601 = i601 / 2;
  i602 = i602 / 2;
  i603 = i603 / 2;
  i604 = i604 / 2;
  i605 = i605 / 2;
  i606 = i606 / 2;
  i607 = i607 / 2;
  i608 = i608 / 2;
  i609 = i609 / 2;
  i610 = i610 / 2;
  i611 = i611 / 2;
  i612 = i612 / 2;
  i613 = i613 / 2;
  i614 = i614 / 2;
  i615 = i615 / 2;
  i616 = i616 / 2;
  i617 = i617 / 2;
  i618 = i618 / 2;
  i619 = i619 / 2;
  i620 = i620 / 2;
  i621 = i621 / 2;
  i622 = i622 / 2;
  i623 = i623 / 2;
  i624 = i624 / 2;
  i625 = i625 / 2;
  i626 = i626 / 2;
  i627 = i627 / 2;
  i628 = i628 / 2;
  i629 = i629 / 2;
  i630 = i630 / 2;
  i631 = i631 / 2;
  i632 = i632 / 2;
  i633 = i633 / 2;
  i634 = i634 / 2;
  i635 = i635 / 2;
  i636 = i636 / 2;
  i637 = i637 / 2;
  i638 = i638 / 2;
  i639 = i639 / 2;
  i640 = i640 / 2;
  i641 = i641 / 2;
  i642 = i642 / 2;
  i643 = i643 / 2;
  i644 = i644 / 2;
  i645 = i645 / 2;
  i646 = i646 / 2;
  i647 = i647 / 2;
  i648 = i648 / 2;
  i649 = i649 / 2;
  i650 = i650 / 2;
  i651 = i651 / 2;
  i652 = i652 / 2;
  i653 = i653 / 2;
  i654 = i654 / 2;
  i655 = i655 / 2;
  i656 = i656 / 2;
  i657 = i657 / 2;
  i658 = i658 / 2;
  i659 = i659 / 2;
  i660 = i660 / 2;
  i661 = i661 / 2;
  i662 = i662 / 2;
  i663 = i663 / 2;
  i664 = i664 / 2;
  i665 = i665 / 2;
  i666 = i666 / 2;
  i667 = i667 / 2;
  i668 = i668 / 2;
  i669 = i669 / 2;
  i670 = i670 / 2;
  i671 = i671 / 2;
  i672 = i672 / 2;
  i673 = i673 / 2;
  i674 = i674 / 2;
  i675 = i675 / 2;
  i676 = i676 / 2;
  i677 = i677 / 2;
  i678 = i678 / 2;
  i679 = i679 / 2;
  i680 = i680 / 2;
  i681 = i681 / 2;
  i682 = i682 / 2;
  i683 = i683 / 2;
  i684 = i684 / 2;
  i685 = i685 / 2;
  i686 = i686 / 2;
  i687 = i687 / 2;
  i688 = i688 / 2;
  i689 = i689 / 2;
  i690 = i690 / 2;
  i691 = i691 / 2;
  i692 = i692 / 2;
  i693 = i693 / 2;
  i694 = i694 / 2;
  i695 = i695 / 2;
  i696 = i696 / 2;
  i697 = i697 / 2;
  i698 = i698 / 2;
  i699 = i699 / 2;
  i700 = i700 / 2;
  i701 = i701 / 2;
  i702 = i702 / 2;
  i703 = i703 / 2;
  i704 = i704 / 2;
  i705 = i705 / 2;
  i706 = i706 / 2;
  i707 = i707 / 2;
  i708 = i708 / 2;
  i709 = i709 / 2;
  i710 = i710 / 2;
  i711 = i711 / 2;
  i712 = i712 / 2;
  i713 = i713 / 2;
  i714 = i714 / 2;
  i715 = i715 / 2;
  i716 = i716 / 2;
  i717 = i717 / 2;
  i718 = i718 / 2;
  i719 = i719 / 2;
  i720 = i720 / 2;
  i721 = i721 / 2;
  i722 = i722 / 2;
  i723 = i723 / 2;
  i724 = i724 / 2;
  i725 = i725 / 2;
  i726 = i726 / 2;
  i727 = i727 / 2;
  i728 = i728 / 2;
  i729 = i729 / 2;
  i730 = i730 / 2;
  i731 = i731 / 2;
  i732 = i732 / 2;
  i733 = i733 / 2;
  i734 = i734 / 2;
  i735 = i735 / 2;
  i736 = i736 / 2;
  i737 = i737 / 2;
  i738 = i738 / 2;
  i739 = i739 / 2;
  i740 = i740 / 2;
  i741 = i741 / 2;
  i742 = i742 / 2;
  i743 = i743 / 2;
  i744 = i744 / 2;
  i745 = i745 / 2;
  i746 = i746 / 2;
  i747 = i747 / 2;
  i748 = i748 / 2;
  i749 = i749 / 2;
  i750 = i750 / 2;
  i751 = i751 / 2;
  i752 = i752 / 2;
  i753 = i753 / 2;
  i754 = i754 / 2;
  i755 = i755 / 2;
  i756 = i756 / 2;
  i757 = i757 / 2;
  i758 = i758 / 2;
  i759 = i759 / 2;
  i760 = i760 / 2;
  i761 = i761 / 2;
  i762 = i762 / 2;
  i763 = i763 / 2;
  i764 = i764 / 2;
  i765 = i765 / 2;
  i766 = i766 / 2;
  i767 = i767 / 2;
  i768 = i768 / 2;
  i769 = i769 / 2;
  i770 = i770 / 2;
  i771 = i771 / 2;
  i772 = i772 / 2;
  i773 = i773 / 2;
  i774 = i774 / 2;
  i775 = i775 / 2;
  i776 = i776 / 2;
  i777 = i777 / 2;
  i778 = i778 / 2;
  i779 = i779 / 2;
  i780 = i780 / 2;
  i781 = i781 / 2;
  i782 = i782 / 2;
  i783 = i783 / 2;
  i784 = i784 / 2;
  i785 = i785 / 2;
  i786 = i786 / 2;
  i787 = i787 / 2;
  i788 = i788 / 2;
  i789 = i789 / 2;
  i790 = i790 / 2;
  i791 = i791 / 2;
  i792 = i792 / 2;
  i793 = i793 / 2;
  i794 = i794 / 2;
  i795 = i795 / 2;
  i796 = i796 / 2;
  i797 = i797 / 2;
  i798 = i798 / 2;
  i799 = i799 / 2;
  i800 = i800 / 2;
  i801 = i801 / 2;
  i802 = i802 / 2;
  i803 = i803 / 2;
  i804 = i804 / 2;
  i805 = i805 / 2;
  i806 = i806 / 2;
  i807 = i807 / 2;
  i808 = i808 / 2;
  i809 = i809 / 2;
  i810 = i810 / 2;
  i811 = i811 / 2;
  i812 = i812 / 2;
  i813 = i813 / 2;
  i814 = i814 / 2;
  i815 = i815 / 2;
  i816 = i816 / 2;
  i817 = i817 / 2;
  i818 = i818 / 2;
  i819 = i819 / 2;
  i820 = i820 / 2;
  i821 = i821 / 2;
  i822 = i822 / 2;
  i823 = i823 / 2;
  i824 = i824 / 2;
  i825 = i825 / 2;
  i826 = i826 / 2;
  i827 = i827 / 2;
  i828 = i828 / 2;
  i829 = i829 / 2;
  i830 = i830 / 2;
  i831 = i831 / 2;
  i832 = i832 / 2;
  i833 = i833 / 2;
  i834 = i834 / 2;
  i835 = i835 / 2;
  i836 = i836 / 2;
  i837 = i837 / 2;
  i838 = i838 / 2;
  i839 = i839 / 2;
  i840 = i840 / 2;
  i841 = i841 / 2;
  i842 = i842 / 2;
  i843 = i843 / 2;
  i844 = i844 / 2;
  i845 = i845 / 2;
  i846 = i846 / 2;
  i847 = i847 / 2;
  i848 = i848 / 2;
  i849 = i849 / 2;
  i850 = i850 / 2;
  i851 = i851 / 2;
  i852 = i852 / 2;
  i853 = i853 / 2;
  i854 = i854 / 2;
  i855 = i855 / 2;
  i856 = i856 / 2;
  i857 = i857 / 2;
  i858 = i858 / 2;
  i859 = i859 / 2;
  i860 = i860 / 2;
  i861 = i861 / 2;
  i862 = i862 / 2;
  i863 = i863 / 2;
  i864 = i864 / 2;
  i865 = i865 / 2;
  i866 = i866 / 2;
  i867 = i867 / 2;
  i868 = i868 / 2;
  i869 = i869 / 2;
  i870 = i870 / 2;
  i871 = i871 / 2;
  i872 = i872 / 2;
  i873 = i873 / 2;
  i874 = i874 / 2;
  i875 = i875 / 2;
  i876 = i876 / 2;
  i877 = i877 / 2;
  i878 = i878 / 2;
  i879 = i879 / 2;
  i880 = i880 / 2;
  i881 = i881 / 2;
  i882 = i882 / 2;
  i883 = i883 / 2;
  i884 = i884 / 2;
  i885 = i885 / 2;
  i886 = i886 / 2;
  i887 = i887 / 2;
  i888 = i888 / 2;
  i889 = i889 / 2;
  i890 = i890 / 2;
  i891 = i891 / 2;
  i892 = i892 / 2;
  i893 = i893 / 2;
  i894 = i894 / 2;
  i895 = i895 / 2;
  i896 = i896 / 2;
  i897 = i897 / 2;
  i898 = i898 / 2;
  i899 = i899 / 2;
  i900 = i900 / 2;
  i901 = i901 / 2;
  i902 = i902 / 2;
  i903 = i903 / 2;
  i904 = i904 / 2;
  i905 = i905 / 2;
  i906 = i906 / 2;
  i907 = i907 / 2;
  i908 = i908 / 2;
  i909 = i909 / 2;
  i910 = i910 / 2;
  i911 = i911 / 2;
  i912 = i912 / 2;
  i913 = i913 / 2;
  i914 = i914 / 2;
  i915 = i915 / 2;
  i916 = i916 / 2;
  i917 = i917 / 2;
  i918 = i918 / 2;
  i919 = i919 / 2;
  i920 = i920 / 2;
  i921 = i921 / 2;
  i922 = i922 / 2;
  i923 = i923 / 2;
  i924 = i924 / 2;
  i925 = i925 / 2;
  i926 = i926 / 2;
  i927 = i927 / 2;
  i928 = i928 / 2;
  i929 = i929 / 2;
  i930 = i930 / 2;
  i931 = i931 / 2;
  i932 = i932 / 2;
  i933 = i933 / 2;
  i934 = i934 / 2;
  i935 = i935 / 2;
  i936 = i936 / 2;
  i937 = i937 / 2;
  i938 = i938 / 2;
  i939 = i939 / 2;
  i940 = i940 / 2;
  i941 = i941 / 2;
  i942 = i942 / 2;
  i943 = i943 / 2;
  i944 = i944 / 2;
  i945 = i945 / 2;
  i946 = i946 / 2;
  i947 = i947 / 2;
  i948 = i948 / 2;
  i949 = i949 / 2;
  i950 = i950 / 2;
  i951 = i951 / 2;
  i952 = i952 / 2;
  i953 = i953 / 2;
  i954 = i954 / 2;
  i955 = i955 / 2;
  i956 = i956 / 2;
  i957 = i957 / 2;
  i958 = i958 / 2;
  i959 = i959 / 2;
  i960 = i960 / 2;
  i961 = i961 / 2;
  i962 = i962 / 2;
  i963 = i963 / 2;
  i964 = i964 / 2;
  i965 = i965 / 2;
  i966 = i966 / 2;
  i967 = i967 / 2;
  i968 = i968 / 2;
  i969 = i969 / 2;
  i970 = i970 / 2;
  i971 = i971 / 2;
  i972 = i972 / 2;
  i973 = i973 / 2;
  i974 = i974 / 2;
  i975 = i975 / 2;
  i976 = i976 / 2;
  i977 = i977 / 2;
  i978 = i978 / 2;
  i979 = i979 / 2;
  i980 = i980 / 2;
  i981 = i981 / 2;
  i982 = i982 / 2;
  i983 = i983 / 2;
  i984 = i984 / 2;
  i985 = i985 / 2;
  i986 = i986 / 2;
  i987 = i987 / 2;
  i988 = i988 / 2;
  i989 = i989 / 2;
  i990 = i990 / 2;
  i991 = i991 / 2;
  i992 = i992 / 2;
  i993 = i993 / 2;
  i994 = i994 / 2;
  i995 = i995 / 2;
  i996 = i996 / 2;
  i997 = i997 / 2;
  i998 = i998 / 2;
  i999 = i999 / 2;
  i1000 = i1000 / 2;
  return i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 + i11 + i12 + i13 +
         i14 + i15 + i16 + i17 + i18 + i19 + i20 + i21 + i22 + i23 + i24 + i25 +
         i26 + i27 + i28 + i29 + i30 + i31 + i32 + i33 + i34 + i35 + i36 + i37 +
         i38 + i39 + i40 + i41 + i42 + i43 + i44 + i45 + i46 + i47 + i48 + i49 +
         i50 + i51 + i52 + i53 + i54 + i55 + i56 + i57 + i58 + i59 + i60 + i61 +
         i62 + i63 + i64 + i65 + i66 + i67 + i68 + i69 + i70 + i71 + i72 + i73 +
         i74 + i75 + i76 + i77 + i78 + i79 + i80 + i81 + i82 + i83 + i84 + i85 +
         i86 + i87 + i88 + i89 + i90 + i91 + i92 + i93 + i94 + i95 + i96 + i97 +
         i98 + i99 + i100 + i101 + i102 + i103 + i104 + i105 + i106 + i107 +
         i108 + i109 + i110 + i111 + i112 + i113 + i114 + i115 + i116 + i117 +
         i118 + i119 + i120 + i121 + i122 + i123 + i124 + i125 + i126 + i127 +
         i128 + i129 + i130 + i131 + i132 + i133 + i134 + i135 + i136 + i137 +
         i138 + i139 + i140 + i141 + i142 + i143 + i144 + i145 + i146 + i147 +
         i148 + i149 + i150 + i151 + i152 + i153 + i154 + i155 + i156 + i157 +
         i158 + i159 + i160 + i161 + i162 + i163 + i164 + i165 + i166 + i167 +
         i168 + i169 + i170 + i171 + i172 + i173 + i174 + i175 + i176 + i177 +
         i178 + i179 + i180 + i181 + i182 + i183 + i184 + i185 + i186 + i187 +
         i188 + i189 + i190 + i191 + i192 + i193 + i194 + i195 + i196 + i197 +
         i198 + i199 + i200 + i201 + i202 + i203 + i204 + i205 + i206 + i207 +
         i208 + i209 + i210 + i211 + i212 + i213 + i214 + i215 + i216 + i217 +
         i218 + i219 + i220 + i221 + i222 + i223 + i224 + i225 + i226 + i227 +
         i228 + i229 + i230 + i231 + i232 + i233 + i234 + i235 + i236 + i237 +
         i238 + i239 + i240 + i241 + i242 + i243 + i244 + i245 + i246 + i247 +
         i248 + i249 + i250 + i251 + i252 + i253 + i254 + i255 + i256 + i257 +
         i258 + i259 + i260 + i261 + i262 + i263 + i264 + i265 + i266 + i267 +
         i268 + i269 + i270 + i271 + i272 + i273 + i274 + i275 + i276 + i277 +
         i278 + i279 + i280 + i281 + i282 + i283 + i284 + i285 + i286 + i287 +
         i288 + i289 + i290 + i291 + i292 + i293 + i294 + i295 + i296 + i297 +
         i298 + i299 + i300 + i301 + i302 + i303 + i304 + i305 + i306 + i307 +
         i308 + i309 + i310 + i311 + i312 + i313 + i314 + i315 + i316 + i317 +
         i318 + i319 + i320 + i321 + i322 + i323 + i324 + i325 + i326 + i327 +
         i328 + i329 + i330 + i331 + i332 + i333 + i334 + i335 + i336 + i337 +
         i338 + i339 + i340 + i341 + i342 + i343 + i344 + i345 + i346 + i347 +
         i348 + i349 + i350 + i351 + i352 + i353 + i354 + i355 + i356 + i357 +
         i358 + i359 + i360 + i361 + i362 + i363 + i364 + i365 + i366 + i367 +
         i368 + i369 + i370 + i371 + i372 + i373 + i374 + i375 + i376 + i377 +
         i378 + i379 + i380 + i381 + i382 + i383 + i384 + i385 + i386 + i387 +
         i388 + i389 + i390 + i391 + i392 + i393 + i394 + i395 + i396 + i397 +
         i398 + i399 + i400 + i401 + i402 + i403 + i404 + i405 + i406 + i407 +
         i408 + i409 + i410 + i411 + i412 + i413 + i414 + i415 + i416 + i417 +
         i418 + i419 + i420 + i421 + i422 + i423 + i424 + i425 + i426 + i427 +
         i428 + i429 + i430 + i431 + i432 + i433 + i434 + i435 + i436 + i437 +
         i438 + i439 + i440 + i441 + i442 + i443 + i444 + i445 + i446 + i447 +
         i448 + i449 + i450 + i451 + i452 + i453 + i454 + i455 + i456 + i457 +
         i458 + i459 + i460 + i461 + i462 + i463 + i464 + i465 + i466 + i467 +
         i468 + i469 + i470 + i471 + i472 + i473 + i474 + i475 + i476 + i477 +
         i478 + i479 + i480 + i481 + i482 + i483 + i484 + i485 + i486 + i487 +
         i488 + i489 + i490 + i491 + i492 + i493 + i494 + i495 + i496 + i497 +
         i498 + i499 + i500 + i501 + i502 + i503 + i504 + i505 + i506 + i507 +
         i508 + i509 + i510 + i511 + i512 + i513 + i514 + i515 + i516 + i517 +
         i518 + i519 + i520 + i521 + i522 + i523 + i524 + i525 + i526 + i527 +
         i528 + i529 + i530 + i531 + i532 + i533 + i534 + i535 + i536 + i537 +
         i538 + i539 + i540 + i541 + i542 + i543 + i544 + i545 + i546 + i547 +
         i548 + i549 + i550 + i551 + i552 + i553 + i554 + i555 + i556 + i557 +
         i558 + i559 + i560 + i561 + i562 + i563 + i564 + i565 + i566 + i567 +
         i568 + i569 + i570 + i571 + i572 + i573 + i574 + i575 + i576 + i577 +
         i578 + i579 + i580 + i581 + i582 + i583 + i584 + i585 + i586 + i587 +
         i588 + i589 + i590 + i591 + i592 + i593 + i594 + i595 + i596 + i597 +
         i598 + i599 + i600 + i601 + i602 + i603 + i604 + i605 + i606 + i607 +
         i608 + i609 + i610 + i611 + i612 + i613 + i614 + i615 + i616 + i617 +
         i618 + i619 + i620 + i621 + i622 + i623 + i624 + i625 + i626 + i627 +
         i628 + i629 + i630 + i631 + i632 + i633 + i634 + i635 + i636 + i637 +
         i638 + i639 + i640 + i641 + i642 + i643 + i644 + i645 + i646 + i647 +
         i648 + i649 + i650 + i651 + i652 + i653 + i654 + i655 + i656 + i657 +
         i658 + i659 + i660 + i661 + i662 + i663 + i664 + i665 + i666 + i667 +
         i668 + i669 + i670 + i671 + i672 + i673 + i674 + i675 + i676 + i677 +
         i678 + i679 + i680 + i681 + i682 + i683 + i684 + i685 + i686 + i687 +
         i688 + i689 + i690 + i691 + i692 + i693 + i694 + i695 + i696 + i697 +
         i698 + i699 + i700 + i701 + i702 + i703 + i704 + i705 + i706 + i707 +
         i708 + i709 + i710 + i711 + i712 + i713 + i714 + i715 + i716 + i717 +
         i718 + i719 + i720 + i721 + i722 + i723 + i724 + i725 + i726 + i727 +
         i728 + i729 + i730 + i731 + i732 + i733 + i734 + i735 + i736 + i737 +
         i738 + i739 + i740 + i741 + i742 + i743 + i744 + i745 + i746 + i747 +
         i748 + i749 + i750 + i751 + i752 + i753 + i754 + i755 + i756 + i757 +
         i758 + i759 + i760 + i761 + i762 + i763 + i764 + i765 + i766 + i767 +
         i768 + i769 + i770 + i771 + i772 + i773 + i774 + i775 + i776 + i777 +
         i778 + i779 + i780 + i781 + i782 + i783 + i784 + i785 + i786 + i787 +
         i788 + i789 + i790 + i791 + i792 + i793 + i794 + i795 + i796 + i797 +
         i798 + i799 + i800 + i801 + i802 + i803 + i804 + i805 + i806 + i807 +
         i808 + i809 + i810 + i811 + i812 + i813 + i814 + i815 + i816 + i817 +
         i818 + i819 + i820 + i821 + i822 + i823 + i824 + i825 + i826 + i827 +
         i828 + i829 + i830 + i831 + i832 + i833 + i834 + i835 + i836 + i837 +
         i838 + i839 + i840 + i841 + i842 + i843 + i844 + i845 + i846 + i847 +
         i848 + i849 + i850 + i851 + i852 + i853 + i854 + i855 + i856 + i857 +
         i858 + i859 + i860 + i861 + i862 + i863 + i864 + i865 + i866 + i867 +
         i868 + i869 + i870 + i871 + i872 + i873 + i874 + i875 + i876 + i877 +
         i878 + i879 + i880 + i881 + i882 + i883 + i884 + i885 + i886 + i887 +
         i888 + i889 + i890 + i891 + i892 + i893 + i894 + i895 + i896 + i897 +
         i898 + i899 + i900 + i901 + i902 + i903 + i904 + i905 + i906 + i907 +
         i908 + i909 + i910 + i911 + i912 + i913 + i914 + i915 + i916 + i917 +
         i918 + i919 + i920 + i921 + i922 + i923 + i924 + i925 + i926 + i927 +
         i928 + i929 + i930 + i931 + i932 + i933 + i934 + i935 + i936 + i937 +
         i938 + i939 + i940 + i941 + i942 + i943 + i944 + i945 + i946 + i947 +
         i948 + i949 + i950 + i951 + i952 + i953 + i954 + i955 + i956 + i957 +
         i958 + i959 + i960 + i961 + i962 + i963 + i964 + i965 + i966 + i967 +
         i968 + i969 + i970 + i971 + i972 + i973 + i974 + i975 + i976 + i977 +
         i978 + i979 + i980 + i981 + i982 + i983 + i984 + i985 + i986 + i987 +
         i988 + i989 + i990 + i991 + i992 + i993 + i994 + i995 + i996 + i997 +
         i998 + i999 + i1000;
}

int main() {
  int sum = 0;
  int i = 0;
  loopCount = getint();
  starttime();
  while (i < loopCount) {
    int tmp = 0;
    int j = 0;
    while (j < 300) {
      tmp =
          tmp +
          func(i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi, i * multi, i * multi,
               i * multi, i * multi, i * multi, i * multi) /
              size;
      j = j + 1;
    }
    tmp = tmp / 300;
    sum = sum + tmp;
    sum = sum % 2147385347;
    i = i + 1;
  }
  stoptime();
  putint(sum);
  putch(10);
  return 0;
}
