//
//  ContentView.swift
//  SafeSnooze
//
//  Created by Sharjeel Arif on 2023-03-31.
//

import SwiftUI

struct ContentView: View {
    
    @ObservedObject private var viewModel = userViewModel()
    
    var body: some View {
        GeometryReader { geometry in
            ZStack {
                NavigationView {
                    ScrollView {
                        LazyVStack(spacing: 16) {
                            ForEach(viewModel.users) { user in
                                UserView(user: user)
                            }
                        }
                    }
                    .allowsHitTesting(false)
                    .padding(.horizontal)
                    .background(Color.pastelBlue.opacity(0.35))
                    .edgesIgnoringSafeArea(.bottom)
                    .navigationBarTitleDisplayMode(.inline)
                    .toolbar {
                        ToolbarItem(placement: .principal) {
                            Text("Your Baby")
                                .font(.system(size: 30, design: .rounded))
                                .foregroundColor(.pastelDarkBlue)
                        }
                    }
                    .onAppear() {
                        self.viewModel.fetchData()
                    }
                }
                .navigationViewStyle(StackNavigationViewStyle()) // Force the same layout on iPad as on iPhone
                
                VStack {
                    Spacer()
                    HStack {
                        Spacer()
                        Image("sleepingchild")
                            .resizable()
                            .scaledToFit()
                            .frame(width: geometry.size.width * (geometry.size.width > geometry.size.height ? 0.4 : 0.6), height: geometry.size.height * (geometry.size.width > geometry.size.height ? 0.4 : 0.6))
                            .padding()
                    }
                }
            }
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
