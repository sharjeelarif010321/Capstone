//
//  SafeSnoozeApp.swift
//  SafeSnooze
//
//  Created by Sharjeel Arif on 2023-03-31.
//

import SwiftUI
import Firebase

@main
struct SafeSnoozeApp: App {
    
    init() {
        FirebaseApp.configure()
    }
    
    var body: some Scene {
        WindowGroup {
            ContentView()
                .environment(\.colorScheme, .light)
        }
    }
}
