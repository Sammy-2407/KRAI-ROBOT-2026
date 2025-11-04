import cv2
import numpy as np
from skimage.filters import threshold_otsu

reference_path = "Fake1.png" 
ref_img = cv2.imread(reference_path, cv2.IMREAD_COLOR)
gray_ref = cv2.cvtColor(ref_img, cv2.COLOR_BGR2GRAY) #convert gambar ke grayscale

#otsu tresholding
thresh_val = threshold_otsu(gray_ref)
_, binary_ref = cv2.threshold(gray_ref, thresh_val, 255, cv2.THRESH_BINARY_INV)


orb = cv2.ORB_create(1000) # feature
kp_ref, des_ref = orb.detectAndCompute(binary_ref, None) #ref dan des point

#crosscheck sama matcher
bf = cv2.BFMatcher(cv2.NORM_HAMMING, crossCheck=True)

cap = cv2.VideoCapture(0)
print("'q'")

while True:
    ret, frame = cap.read()
    if not ret:
        break

    gray_cam = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY) #convert input camera ke grayscale
    t_val = threshold_otsu(gray_cam) #treshold graycam
    _, binary_cam = cv2.threshold(gray_cam, t_val, 255, cv2.THRESH_BINARY_INV)

    kp_cam, des_cam = orb.detectAndCompute(binary_cam, None) #description point dari camera

    # Hanya jika fitur ditemukan di kedua gambar
    if des_cam is not None and des_ref is not None:
        matches = bf.match(des_ref, des_cam)
        matches = sorted(matches, key=lambda x: x.distance)

        # Ambil sebagian match terbaik
        good_matches = [m for m in matches if m.distance < 70]

        # Tampilkan hasil di layar
        matched_img = cv2.drawMatches(ref_img, kp_ref, frame, kp_cam, good_matches[:20], None, flags=2)

        match_score = len(good_matches)
        cv2.putText(matched_img, f"Matches: {match_score}", (10, 30),
                    cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0) if match_score > 30 else (0, 0, 255), 2)

        if match_score > 50: #standar match score
            cv2.putText(matched_img, "MATCH!", (10, 70),
                        cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 3)
        else:
            cv2.putText(matched_img, "NOT MATCH", (10, 70),
                        cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 3)

        cv2.imshow("Matching Result", matched_img)
    else:
        cv2.imshow("Matching Result", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()