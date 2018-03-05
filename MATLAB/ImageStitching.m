%Left and Middle Images

middle_image_points1 = [ 975 978 1077 1079 1018 1103 992 1160;  1117 1210 1105 1199 660 1062 834 845];
left_image_points = [ 277 279 371 373 322 398 295 465;  467 557 470 563 31 429 203 200];

middle_XY1 = [middle_image_points1(1,:) + 0.5; 1920 - 0.5 - middle_image_points1(2,:)];
left_XY = [left_image_points(1,:) + 0.5; 640 - 0.5 - left_image_points(2,:)];

H1 = homography(left_XY, middle_XY1);
H_inv1 = inv(H1);

%Right and Middle Images
middle_image_points2 = [1288 1293 1345 1350 1208 1279 1223 1226;  1134 1209 1138 1218 846  1076  1029 1067];
right_image_points = [134 138 189 193 47 125 66 69;  493 567 491 568 200 436 392 432];

middle_XY2 = [middle_image_points2(1,:) + 0.5; 1920 - 0.5 - middle_image_points2(2,:)];
right_XY = [right_image_points(1,:) + 0.5; 640 - 0.5 - right_image_points(2,:)];

H2 = homography(right_XY, middle_XY2);
H_inv2 = inv(H2);