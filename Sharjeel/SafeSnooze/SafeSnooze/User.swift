//
//  User.swift
//  SafeSnooze
//
//  Created by Sharjeel Arif on 2023-03-31.
//

import Foundation

struct User: Identifiable {
    var id: String = UUID().uuidString
    var heartrate: String
    var position: String
    var posture: String
    var spo2: String
    var status: String
}
