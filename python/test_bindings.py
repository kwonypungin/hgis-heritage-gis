#!/usr/bin/env python3
"""
HGIS Python 바인딩 테스트
"""

import sys
import os

# 빌드 디렉토리를 Python 경로에 추가
sys.path.insert(0, '/home/dk/hgis/build/python')
sys.path.insert(0, '/home/dk/hgis/python')

print("====================================")
print("HGIS Python 바인딩 테스트")
print("====================================\n")

# hgis 패키지 테스트
try:
    import hgis
    print("✓ hgis 패키지 로드 성공")
    print(f"  버전: {hgis.version()}")
    print(f"  릴리즈: {hgis.releaseName()}")
except ImportError as e:
    print(f"✗ hgis 패키지 로드 실패: {e}")

# hgis_core 확장 모듈 테스트
try:
    import hgis_core
    print("\n✓ hgis_core 확장 모듈 로드 성공")
    
    # 버전 테스트
    version = hgis_core.version()
    print(f"  HGIS 버전: {version}")
    
    # CRS 생성 테스트
    print("\n좌표계 테스트:")
    
    # WGS84
    crs_wgs84 = hgis_core.create_crs(4326)
    epsg_wgs84 = hgis_core.crs_epsg(crs_wgs84)
    print(f"  ✓ WGS84 생성: EPSG:{epsg_wgs84}")
    
    # Korea 2000 Central Belt
    crs_korea = hgis_core.create_crs(5186)
    epsg_korea = hgis_core.crs_epsg(crs_korea)
    print(f"  ✓ Korea 2000 Central Belt 생성: EPSG:{epsg_korea}")
    
    # UTM-K
    crs_utmk = hgis_core.create_crs(5179)
    epsg_utmk = hgis_core.crs_epsg(crs_utmk)
    print(f"  ✓ UTM-K 생성: EPSG:{epsg_utmk}")
    
    print("\n모든 테스트 통과! ✓")
    
except ImportError as e:
    print(f"\n✗ hgis_core 모듈 로드 실패: {e}")
    import traceback
    traceback.print_exc()
except Exception as e:
    print(f"\n✗ 테스트 중 오류 발생: {e}")
    import traceback
    traceback.print_exc()

print("\n====================================")
print("테스트 완료")
print("====================================")

# 사용 예제
print("\n사용 예제:")
print("```python")
print("import hgis_core")
print("")
print("# 좌표계 생성")
print("crs = hgis_core.create_crs(5186)  # Korea 2000 Central Belt")
print("epsg = hgis_core.crs_epsg(crs)")
print("print(f'EPSG: {epsg}')")
print("```")